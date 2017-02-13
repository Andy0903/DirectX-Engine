#include "Direct3DClass.h"

Direct3DClass::Direct3DClass()
{
	mySwapChain = nullptr;
	myDevice = nullptr;
	myDeviceContext = nullptr;
	myRenderTargetView = nullptr;
	myDepthStencilBuffer = nullptr;
	myDepthStencilState = nullptr;
	myDepthStencilView = nullptr;
	myRasterState = nullptr;
	myDepthDisabledStencilState = nullptr;
}

Direct3DClass::Direct3DClass(const Direct3DClass& aOther)
{
}

Direct3DClass::~Direct3DClass()
{
}

bool Direct3DClass::Initialize(int aScreenWidth, int aScreenHeight, bool aVsync, HWND aHwnd, bool aFullscreen, float aScreenDepth, float aScreenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes;
	unsigned int numerator;
	unsigned int denominator;
	unsigned int stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBuffer;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView;
	float screenAspect;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;

	myVsyncEnabled = aVsync;

	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result)) { return false; }

	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result)) { return false; }

	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result)) { return false; }

	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
	if (FAILED(result)) { return false; }

	displayModeList = new DXGI_MODE_DESC[numModes];

	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result)) { return false; }

	for (int i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)aScreenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)aScreenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result)) { return false; }

	myVideoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	error = wcstombs_s(&stringLength, myVideoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0) { return false; }

	delete[] displayModeList;
	displayModeList = nullptr;

	adapterOutput->Release();
	adapterOutput = nullptr;

	adapter->Release();
	adapter = nullptr;

	factory->Release();
	factory = nullptr;

	SecureZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = aScreenWidth;
	swapChainDesc.BufferDesc.Height = aScreenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (myVsyncEnabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	swapChainDesc.OutputWindow = aHwnd;

	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	if (aFullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	swapChainDesc.Flags = 0;

	featureLevel = D3D_FEATURE_LEVEL_11_0;

	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &mySwapChain, &myDevice, nullptr, &myDeviceContext);
	if (FAILED(result)) { return false; }

	result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if (FAILED(result)) { return false; }

	result = myDevice->CreateRenderTargetView(backBuffer, nullptr, &myRenderTargetView);
	if (FAILED(result)) { return false; }

	backBuffer->Release();
	backBuffer = nullptr;

	SecureZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = aScreenWidth;
	depthBufferDesc.Height = aScreenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	result = myDevice->CreateTexture2D(&depthBufferDesc, nullptr, &myDepthStencilBuffer);
	if (FAILED(result)) { return false; }

	SecureZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = myDevice->CreateDepthStencilState(&depthStencilDesc, &myDepthStencilState);
	if (FAILED(result)) { return false; }

	myDeviceContext->OMSetDepthStencilState(myDepthStencilState, 1);

	SecureZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = myDevice->CreateDepthStencilView(myDepthStencilBuffer, &depthStencilViewDesc, &myDepthStencilView);
	if (FAILED(result)) { return false; }

	myDeviceContext->OMSetRenderTargets(1, &myRenderTargetView, myDepthStencilView);

	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	result = myDevice->CreateRasterizerState(&rasterDesc, &myRasterState);
	if (FAILED(result)) { return false; }

	myDeviceContext->RSSetState(myRasterState);

	viewport.Width = (float)aScreenWidth;
	viewport.Height = (float)aScreenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	myDeviceContext->RSSetViewports(1, &viewport);

	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = (float)aScreenWidth / (float)aScreenHeight;
	D3DXMatrixPerspectiveFovLH(&myProjectionMatrix, fieldOfView, screenAspect, aScreenNear, aScreenDepth);

	D3DXMatrixIdentity(&myWorldMatrix);
	D3DXMatrixOrthoLH(&myOrthoMatrix, (float)aScreenWidth, (float)aScreenHeight, aScreenNear, aScreenDepth);

	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the state using the device.
	result = myDevice->CreateDepthStencilState(&depthDisabledStencilDesc, &myDepthDisabledStencilState);
	if (FAILED(result)) { return false; }

	return true;
}

void Direct3DClass::Shutdown()
{
	//Needs to be on windowed before release, else throws exception.
	if (mySwapChain)
	{
		mySwapChain->SetFullscreenState(false, nullptr);
	}

	if (myDepthDisabledStencilState)
	{
		myDepthDisabledStencilState->Release();
		myDepthDisabledStencilState = nullptr;
	}

	if (myRasterState)
	{
		myRasterState->Release();
		myRasterState = nullptr;
	}

	if (myDepthStencilView)
	{
		myDepthStencilView->Release();
		myDepthStencilView = nullptr;
	}

	if (myDepthStencilState)
	{
		myDepthStencilState->Release();
		myDepthStencilState = nullptr;
	}

	if (myDepthStencilBuffer)
	{
		myDepthStencilBuffer->Release();
		myDepthStencilBuffer = nullptr;
	}

	if (myRenderTargetView)
	{
		myRenderTargetView->Release();
		myRenderTargetView = nullptr;
	}

	if (myDeviceContext)
	{
		myDeviceContext->Release();
		myDeviceContext = nullptr;
	}

	if (myDevice)
	{
		myDevice->Release();
		myDevice = nullptr;
	}

	if (mySwapChain)
	{
		mySwapChain->Release();
		mySwapChain = nullptr;
	}
}

void Direct3DClass::BeginScene(float aRed, float aGreen, float aBlue, float aAlpha)
{
	float color[4]{ aRed, aGreen, aBlue, aAlpha };

	myDeviceContext->ClearRenderTargetView(myRenderTargetView, color);
	myDeviceContext->ClearDepthStencilView(myDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Direct3DClass::EndScene()
{
	mySwapChain->Present(myVsyncEnabled ? 1 : 0, 0);
}

ID3D11Device* Direct3DClass::GetDevice()
{
	return myDevice;
}

ID3D11DeviceContext* Direct3DClass::GetDeviceContext()
{
	return myDeviceContext;
}

void Direct3DClass::GetProjectionMatrix(D3DXMATRIX& aProjectionMatrix)
{
	aProjectionMatrix = myProjectionMatrix;
}


void Direct3DClass::GetWorldMatrix(D3DXMATRIX& aWorldMatrix)
{
	aWorldMatrix = myWorldMatrix;
}


void Direct3DClass::GetOrthoMatrix(D3DXMATRIX& aOrthoMatrix)
{
	aOrthoMatrix = myOrthoMatrix;
}

void Direct3DClass::GetVideoCardInfo(char* aCardName, int& aMemory)
{
	strcpy_s(aCardName, 128, myVideoCardDescription);
	aMemory = myVideoCardMemory;
}

void Direct3DClass::TurnZBufferOn()
{
	myDeviceContext->OMSetDepthStencilState(myDepthStencilState, 1);
}


void Direct3DClass::TurnZBufferOff()
{
	myDeviceContext->OMSetDepthStencilState(myDepthDisabledStencilState, 1);
}