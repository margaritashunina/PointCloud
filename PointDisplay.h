#pragma once

#include <iostream>
#include <fstream>
#include <chrono>
#include <random>

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace DirectX;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Platform;

const int NMAX = 300000;

const float PI = acos(-1.);

struct vertex {
	float x, y, z;
};

struct matrix {
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

Array<byte>^ loadShaderFiles(std::string File);
float getRand(std::mt19937& rng);

class PointDisplay{
public:
	void Initialaze();
	void Update(float dx, float dy, float dRaw, float dPitch);
	void Render();
	void newSetOfPoints();

private:
	void InitGraphics();
	void InitPipeline();
	void InitMatrix();

	void UpdateViewMatrix();
	void UpdateMatrixBuffer();
	

	ComPtr<ID3D11Device1> device;
	ComPtr<ID3D11DeviceContext1> context;
	ComPtr<IDXGISwapChain1> swapchain;
	ComPtr<ID3D11RenderTargetView> rendertarget;
	ComPtr<ID3D11Buffer> vertexbuffer;
	ComPtr<ID3D11Buffer> matrixbuffer;
	ComPtr<ID3D11VertexShader> vertexshader;
	ComPtr<ID3D11PixelShader> pixelshader;
	ComPtr<ID3D11InputLayout> inputlayout;

	matrix mBufferData;
	std::vector<int> displayIDs;
	float pitch, raw;
	DirectX::XMFLOAT4 viewPoint;
	DirectX::XMFLOAT4 viewVec;
};

