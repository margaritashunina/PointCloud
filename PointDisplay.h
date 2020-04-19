#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <random>
#include "scene.hpp"

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

const int M = 222;
const int N = M * M * 6 / 4;
const int NMAX = 2 * M * M + N;

//const float PI = acos(-1.);

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
	void Update(float df, float dlr, float dRaw, float dPitch);
	void Render();
	void newSetOfPoints();

	ComPtr<ID3D11DeviceContext1> context;
	//FUN
	//void UpdateColors();

private:
	void InitScene();
	void InitGraphics();
	void InitDepthStencil();
	void InitPipeline();
	void InitMatrix();

	void UpdateViewMatrix();
	void UpdateMatrixBuffer();
	void UpdateScene(float dx, float dy, float dz);
	

	ComPtr<ID3D11Device1> device;
	
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
	Scene KDscene;
	
	//FUN
	//std::vector<vertex> colors;
	//int colorIt;
	//void InitColors();
};

