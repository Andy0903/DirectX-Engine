#ifndef DIRECT3DCLASS_H
#define DIRECT3DCLASS_H

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <D3DX10math.h>

class Direct3DClass
{
public:
	Direct3DClass();
	Direct3DClass(const Direct3DClass&);
	~Direct3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(D3DXMATRIX&);
	void GetWorldMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);

	void GetVideoCardInfo(char*, int&);

	void TurnZBufferOn();
	void TurnZBufferOff();

	void ResetViewport();

	ID3D11DepthStencilView* GetDepthStencilView();
	void SetBackBufferRenderTarget();

private:
	bool myVsyncEnabled;
	int myVideoCardMemory;
	char myVideoCardDescription[128];
	IDXGISwapChain *mySwapChain;
	ID3D11Device *myDevice;
	ID3D11DeviceContext *myDeviceContext;
	ID3D11RenderTargetView *myRenderTargetView;
	ID3D11Texture2D *myDepthStencilBuffer;
	ID3D11DepthStencilState *myDepthStencilState;
	ID3D11DepthStencilView *myDepthStencilView;
	ID3D11RasterizerState *myRasterState;
	D3DXMATRIX myProjectionMatrix;
	D3DXMATRIX myWorldMatrix;
	D3DXMATRIX myOrthoMatrix;

	ID3D11DepthStencilState *myDepthDisabledStencilState;
	ID3D11BlendState *myAlphaEnableBlendingState;
	ID3D11BlendState *myAlphaDisableBlendingState;

	D3D11_VIEWPORT myViewport;
};

#endif