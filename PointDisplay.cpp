#include "pch.h"
#include "PointDisplay.h"

//------EXTRA FUNCTIONS-------

//function for loading shader files 
Array<byte>^ loadShaderFiles(std::string File) {
    Array<byte>^ shaderData = nullptr;
    std::ifstream shaderFile(File, std::ios::in | std::ios::binary | std::ios::ate);

    if (shaderFile.is_open()) {
        int length = (int)shaderFile.tellg();

        shaderData = ref new Array<byte>(length);
        shaderFile.seekg(0, std::ios::beg);
        shaderFile.read(reinterpret_cast<char*>(shaderData->Data), length);
        shaderFile.close();
    }

    return shaderData;
}

float getRand(std::mt19937& rng) {
    int x = rng() % NMAX;
    float xx = (float)x / (float)NMAX;
    int sign = rng() % 2;
    //xx /= 5.;
    if (sign) {
        return xx * -1.0;
    }
    else {
        return xx;
    }
}

void PointDisplay::newSetOfPoints() {
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

    displayIDs.clear();
    int n = rng() % NMAX + 1;
    for (int i = 0; i < n; ++i) {
        int it = rng() % NMAX;
        displayIDs.push_back(it);
    }
}

//------INITIALIZATION FUNCTIONS-------

void PointDisplay::Initialaze() {
    //we have to initialaze DirectX11 device and device context pointers, 
    //but then we convert this to DirectX11.1 device and device context pointers
	ComPtr<ID3D11Device> dev;
	ComPtr<ID3D11DeviceContext> cont;

    D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &dev,
        nullptr,
        &cont);

    dev.As(&device);
    cont.As(&context);
    
    //initialization of special factory
    ComPtr<IDXGIDevice1> dxgiDevice;
    ComPtr<IDXGIAdapter> dxgiAdapter;
    ComPtr<IDXGIFactory2> dxgiFactory;

    device.As(&dxgiDevice);
    dxgiDevice->GetAdapter(&dxgiAdapter);
    dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory);

    //initialization of swapchain
    DXGI_SWAP_CHAIN_DESC1 description = { 0 };
    description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    description.BufferCount = 2;
    description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    description.SampleDesc.Count = 1;
    
    CoreWindow^ window = CoreWindow::GetForCurrentThread();

    dxgiFactory->CreateSwapChainForCoreWindow(
        device.Get(),
        reinterpret_cast<IUnknown*>(window),
        &description,
        nullptr,
        &swapchain
    );
    
    //initialization of target view ans backbuffer
    ComPtr<ID3D11Texture2D> backbuffer;
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backbuffer);
    device->CreateRenderTargetView(backbuffer.Get(), nullptr, &rendertarget);

    D3D11_VIEWPORT viewport = { 0 };
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = window->Bounds.Width;
    viewport.Height = window->Bounds.Height;

    context->RSSetViewports(1, &viewport);

    InitGraphics();
    InitPipeline();
}

void PointDisplay::InitGraphics() {
    viewPoint = DirectX::XMFLOAT4(0, 0, 0, 0);
    viewVec = DirectX::XMFLOAT4(0, 0, 0, 0);
    viewVec.z = 1.;
    pitch = 0;
    raw = 0;
    vertex* sample = new vertex[NMAX];

    float a = -1.0, b = -0.5;
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> dis(a, b);

    float dx = 1. / M, dy = dx;
    float r = 0.2, R = 0.5 - r;
    int id = 0;
    for (int i = -M/2; i < M/2; ++i) {
        for (int j = -M/2; j < M/2; ++j) {
            float x = i * dx;
            float y = j * dy;
            sample[id].x = x;
            sample[id].y = y;
            sample[id].z = sqrt(r * r - R * R - x * x - y * y + 2 * R * sqrt(x * x + y * y));
            ++id;

            sample[id].x = x;
            sample[id].y = y;
            sample[id].z = -sqrt(r * r - R * R - x * x - y * y + 2 * R * sqrt(x * x + y * y));
            ++id;
        }
    }

    for (int i = 0; i < N; i += 6) {
        sample[id].x = a;
        sample[id].y = dis(rng);
        sample[id].z = dis(rng);

        sample[id + 1].x = b;
        sample[id + 1].y = dis(rng);
        sample[id + 1].z = dis(rng);

        sample[id + 2].x = dis(rng);
        sample[id + 2].y = a;
        sample[id + 2].z = dis(rng);

        sample[id + 3].x = dis(rng);
        sample[id + 3].y = b;
        sample[id + 3].z = dis(rng);

        sample[id + 4].x = dis(rng);
        sample[id + 4].y = dis(rng);
        sample[id + 4].z = a;

        sample[id + 5].x = dis(rng);
        sample[id + 5].y = dis(rng);
        sample[id + 5].z = b;

        id += 6;
    }

    D3D11_BUFFER_DESC bufDescription = { 0 }; 
    bufDescription.ByteWidth = sizeof(vertex) * NMAX;
    bufDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA subResData = { sample, 0, 0 };
    device->CreateBuffer(&bufDescription, &subResData, &vertexbuffer);

    delete[] sample;
}

void PointDisplay::InitMatrix() {
    mBufferData.view = DirectX::XMFLOAT4X4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);

    CoreWindow^ window = CoreWindow::GetForCurrentThread();
    float xScale = 1.42814801f;
    float yScale = 1.42814801f;
    float height = window->Bounds.Height;
    float width = window->Bounds.Width;

    if (width > height) {
        xScale = yScale * height / width;
    }
    else {
        yScale = xScale * width / height;
    }

    mBufferData.projection = DirectX::XMFLOAT4X4(
        xScale, 0.0f, 0.0f, 0.0f,
        0.0f, yScale, 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f, -0.01f,
        0.0f, 0.0f, -1.0f, 0.0f
    );
}

void PointDisplay::InitPipeline() {
    Array<byte>^ vertexShaderFile = loadShaderFiles("VertexShader.cso");
    Array<byte>^ pixelShaderFile = loadShaderFiles("PixelShader.cso");

    device->CreateVertexShader(vertexShaderFile->Data, vertexShaderFile->Length, nullptr, &vertexshader);
    device->CreatePixelShader(pixelShaderFile->Data, pixelShaderFile->Length, nullptr, &pixelshader);

    //MATRIX DESCRIPTION VERY IMPORTANT DO NOT TOUCH 
    D3D11_BUFFER_DESC matrixBufferDesc = { 0 };
    matrixBufferDesc.ByteWidth = sizeof(mBufferData);
    matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = 0;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    InitMatrix();

    D3D11_SUBRESOURCE_DATA matixBufferSubData = { &mBufferData, 0, 0 };

    device->CreateBuffer(
        &matrixBufferDesc,
        &matixBufferSubData,
        &matrixbuffer
    );

    context->VSSetShader(vertexshader.Get(), nullptr, 0);
    context->PSSetShader(pixelshader.Get(), nullptr, 0);
    context->VSSetConstantBuffers(0, 1, matrixbuffer.GetAddressOf());
    //context->PSSetConstantBuffers(0, 1, matrixbuffer.GetAddressOf());
    //MAYBE PSSetConstantBuffers too

    D3D11_INPUT_ELEMENT_DESC inputDescrip[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    device->CreateInputLayout(inputDescrip, ARRAYSIZE(inputDescrip), vertexShaderFile->Data, vertexShaderFile->Length, &inputlayout);
    context->IASetInputLayout(inputlayout.Get());
}


//------UPDATE FUNCTIONS-------

void PointDisplay::UpdateMatrixBuffer() {
    context->UpdateSubresource(matrixbuffer.Get(), 0, 0, &mBufferData, 0, 0);
    context->VSSetConstantBuffers(0, 1, matrixbuffer.GetAddressOf());
}

void PointDisplay::UpdateViewMatrix() {
    float radPitch = pitch * PI / 180.;
    float radRaw = raw * PI / 180.;

    float cosPitch = cos(radPitch);
    float sinPitch = sin(radPitch);
    float cosRaw = cos(radRaw);
    float sinRaw = sin(radRaw);

    float scalarX = viewPoint.x * cosRaw - viewPoint.z * sinRaw;
    float scalarY = viewPoint.x * sinPitch * sinRaw + viewPoint.y * cosPitch + viewPoint.z * sinPitch * cosRaw;
    float scalarZ = viewPoint.x * sinRaw * cosPitch - viewPoint.y * sinPitch + viewPoint.z * cosPitch * cosRaw;

    mBufferData.view = DirectX::XMFLOAT4X4(
        cosRaw,               0,        -sinRaw,          scalarX,
        sinPitch * sinRaw, cosPitch,    sinPitch * cosRaw,  scalarY,
        cosPitch * sinRaw, -sinPitch,   cosPitch * cosRaw,  scalarZ, 
        0, 0, 0, 1
    );
}

void PointDisplay::Update(float df, float dlr, float dRaw, float dPitch) {
    pitch += dPitch;
    if (pitch > 85.) {
        pitch = 85.;
    }
    if (pitch < -85.) {
        pitch = -85.;
    }

    raw += dRaw;
    if (raw > 360.) {
        raw -= 360.;
    }
    if (raw < 0.) {
        raw += 360.;
    }

    float radPitch = pitch * PI / 180.;
    float radRaw = raw * PI / 180.;

    float cosPitch = cos(radPitch);
    float sinPitch = sin(radPitch);
    float cosRaw = cos(radRaw);
    float sinRaw = sin(radRaw);

    viewVec.z = cosRaw * cosPitch;
    viewVec.x = sinRaw * cosPitch;
    viewVec.y = sinPitch;

    viewPoint.x += viewVec.x * df;
    viewPoint.y -= viewVec.y * df;
    viewPoint.z += viewVec.z * df;

    float sideVecx = -viewVec.z;
    float sideVecz = viewVec.x;
    float len = sqrt(sideVecx * sideVecx + sideVecz * sideVecz);
    
    sideVecx /= len;
    sideVecz /= len;

    viewPoint.x += sideVecx * dlr;
    viewPoint.z += sideVecz * dlr;

    UpdateViewMatrix();
    UpdateMatrixBuffer();
}

void PointDisplay::Render() {
    context->OMSetRenderTargets(1, rendertarget.GetAddressOf(), nullptr);

    //float color[] = {0, 0, 0, 1.0}; //BACKGROUND COLOR
    float color[] = {1.0, 1.0, 1.0, 1.0}; //BACKGROUND COLOR
    context->ClearRenderTargetView(rendertarget.Get(), color);

    UINT vertexSize = sizeof(vertex);
    UINT zero = 0;

    context->IASetVertexBuffers(0, 1, vertexbuffer.GetAddressOf(), &vertexSize, &zero);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    for (int u : displayIDs) {
        context->Draw(1, u);
    }

    swapchain->Present(1, 0);
}