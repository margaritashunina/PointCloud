#include "pch.h"
#include "PointDisplay.h"

//------EXTRA FUNCTIONS-------

//function for loading shader files 
Array<byte>^ loadShaderFiles(std::string File) {
    Array<byte>^ shaderData = nullptr;
    std::ifstream shaderFile(File, std::ios::in | std::ios::binary | std::ios::ate);
    Platform::String^ localfolder = Windows::Storage::ApplicationData::Current->LocalFolder->Path;

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

    InitScene();
}

void PointDisplay::InitScene() {
    KDscene.setUwpPaint(context);

    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    //KDscene.randomData(NMAX, rng);

    std::ifstream dataFile("C:/Users/jason/AppData/Local/Packages/10960d78-4751-4bb8-8ded-94074895938b_t9sbtj4kj4cst/LocalState/output.txt", std::ios::in);

    if (dataFile.is_open()) {
        KDscene.readScene(dataFile);
    }

    KDscene.initScene();

    int sz = NMAX;
    const pt* dt = KDscene.getData(sz);

    vertex* fdt = new vertex[sz];
    for (int i = 0; i < sz; ++i) {
        fdt[i].x = dt[i].x_;
        fdt[i].y = dt[i].y_;
        fdt[i].z = dt[i].z_;
    }

    D3D11_BUFFER_DESC bufDescription = { 0 };
    bufDescription.ByteWidth = sizeof(vertex) * sz;
    bufDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA subResData = { fdt, 0, 0 };
    device->CreateBuffer(&bufDescription, &subResData, &vertexbuffer);

    KDscene.deleteData();
    delete[] fdt;
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
    float height = 9.; //window->Bounds.Height;
    float width = 16.; //window->Bounds.Width;

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

void PointDisplay::InitDepthStencil() {

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

    //FUN 
    //InitColors();

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
        {"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
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

void PointDisplay::UpdateScene(float dx, float dy, float dz) {
    KDscene.moveViewer(dx, dy, dz);
    KDscene.setViewVector(pt(viewVec.x, viewVec.y, viewVec.z));
}

void PointDisplay::Update(float df, float dlr, float dRaw, float dPitch) {
    pitch += dPitch;
    if (pitch > 90. - 180. / 36.) {
        pitch = 90. - 180. / 36.;
    }
    if (pitch < -(90. - 180. / 36.)) {
        pitch = -(90. - 180. / 36.);
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

    DirectX::XMFLOAT4 oldPoint = viewPoint;

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

    UpdateScene((oldPoint.x - viewPoint.x), (oldPoint.y - viewPoint.y), (oldPoint.z - viewPoint.z));
    UpdateViewMatrix();
    UpdateMatrixBuffer();
}

void PointDisplay::Render() {
    context->OMSetRenderTargets(1, rendertarget.GetAddressOf(), nullptr);

    float color[] = {0, 0, 0, 1.0}; //BACKGROUND COLOR
    //float color[] = {1.0, 1.0, 1.0, 1.0}; //BACKGROUND COLOR
    //float color[] = {colors[colorIt].x / 255., colors[colorIt].y / 255., colors[colorIt].z / 255., 1.0}; //BACKGROUND COLOR

    context->ClearRenderTargetView(rendertarget.Get(), color);

    UINT vertexSize = sizeof(vertex);
    UINT zero = 0;

    context->IASetVertexBuffers(0, 1, vertexbuffer.GetAddressOf(), &vertexSize, &zero);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    //context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //pt jojoVec = KDscene.getViewVector();
    //pt jojoPos = KDscene.getPosViewer();

    KDscene.calcFrame();
    //context->Draw(NMAX, 0);
    swapchain->Present(1, 0);
}

//FUN
/*
void PointDisplay::InitColors() {
    colorIt = 0;
    for (int i = 0; i <= 255; ++i) {
        colors.push_back({ 255., (float)i, 0. });
    }
    for (int i = 255; i >= 0; --i) {
        colors.push_back({ (float)i, 255., 0. });
    }
    for (int i = 0; i <= 255; ++i) {
        colors.push_back({ 0., 255, (float)i });
    }
    for (int i = 255; i >= 0; --i) {
        colors.push_back({ 0., (float)i, 255. });
    }
    for (int i = 0; i <= 255; ++i) {
        colors.push_back({ (float)i, 0., 255. });
    }
    for (int i = 255; i >= 0; --i) {
        colors.push_back({ 255., 0., (float)i });
    }
}

void PointDisplay::UpdateColors() {
    colorIt = (colorIt + 1) % colors.size();
}
*/