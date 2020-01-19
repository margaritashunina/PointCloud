#include "pch.h"
#include "PointDisplay.h"

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
    if (sign) {
        return xx * -1.0;
    }
    else {
        return xx;
    }
}

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
    // old version with a triangle
    vertex sample[NMAX];

    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    for (int i = 0; i < NMAX; ++i) {
        sample[i].x = getRand(rng);
        sample[i].y = getRand(rng);
        sample[i].z = 0.0;
    }

    D3D11_BUFFER_DESC bufDescription = { 0 }; 
    bufDescription.ByteWidth = sizeof(vertex) * ARRAYSIZE(sample);
    bufDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA subResData = { sample, 0, 0 };
    device->CreateBuffer(&bufDescription, &subResData, &vertexbuffer);
}

//reading shaders from files and setting them up
void PointDisplay::InitPipeline() {
    Array<byte>^ vertexShaderFile = loadShaderFiles("VertexShader.cso");
    Array<byte>^ pixelShaderFile = loadShaderFiles("PixelShader.cso");

    device->CreateVertexShader(vertexShaderFile->Data, vertexShaderFile->Length, nullptr, &vertexshader);
    device->CreatePixelShader(pixelShaderFile->Data, pixelShaderFile->Length, nullptr, &pixelshader);

    context->VSSetShader(vertexshader.Get(), nullptr, 0);
    context->PSSetShader(pixelshader.Get(), nullptr, 0);

    D3D11_INPUT_ELEMENT_DESC inputDescrip[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    device->CreateInputLayout(inputDescrip, ARRAYSIZE(inputDescrip), vertexShaderFile->Data, vertexShaderFile->Length, &inputlayout);
    context->IASetInputLayout(inputlayout.Get());
}

void PointDisplay::Update() {


}

void PointDisplay::Render() {
    context->OMSetRenderTargets(1, rendertarget.GetAddressOf(), nullptr);

    float color[] = {1.0f, 0.851f, 0.894f, 1.0f};
    context->ClearRenderTargetView(rendertarget.Get(), color);

    // old things with triangles
    UINT vertexSize = sizeof(vertex);
    UINT zero = 0;

    context->IASetVertexBuffers(0, 1, vertexbuffer.GetAddressOf(), &vertexSize, &zero);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    int it = 0;
    while (it < NMAX) {
        context->Draw(3, it);
        it += 3;
    }

    swapchain->Present(1, 0);
}
