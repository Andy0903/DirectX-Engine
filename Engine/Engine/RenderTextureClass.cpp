#include "RenderTextureClass.h"

RenderTextureClass::RenderTextureClass()
{
	myRenderTargetTexture = nullptr;
	myRenderTargetView = nullptr;
	myShaderResourceView = nullptr;
}

RenderTextureClass::RenderTextureClass(const RenderTextureClass& aOther)
{
}

RenderTextureClass::~RenderTextureClass()
{
}

bool RenderTextureClass::Initialize(ID3D11Device *aDevice, int aTextureWidth, int aTextureHeight)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	// Initialize and Setup the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = aTextureWidth;
	textureDesc.Height = aTextureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the render target texture.
	result = aDevice->CreateTexture2D(&textureDesc, nullptr, &myRenderTargetTexture);
	if (FAILED(result)) { return false; }

	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = aDevice->CreateRenderTargetView(myRenderTargetTexture, &renderTargetViewDesc, &myRenderTargetView);
	if (FAILED(result)) { return false; }

	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = aDevice->CreateShaderResourceView(myRenderTargetTexture, &shaderResourceViewDesc, &myShaderResourceView);
	if (FAILED(result)) { return false; }

	return true;
}

void RenderTextureClass::Shutdown()
{
	if (myShaderResourceView)
	{
		myShaderResourceView->Release();
		myShaderResourceView = nullptr;
	}

	if (myRenderTargetView)
	{
		myRenderTargetView->Release();
		myRenderTargetView = nullptr;
	}

	if (myRenderTargetTexture)
	{
		myRenderTargetTexture->Release();
		myRenderTargetTexture = nullptr;
	}
}

void RenderTextureClass::SetRenderTarget(ID3D11DeviceContext *aDeviceContext, ID3D11DepthStencilView *aDepthStencilView)
{
	aDeviceContext->OMSetRenderTargets(1, &myRenderTargetView, aDepthStencilView);
}

void RenderTextureClass::ClearRenderTarget(ID3D11DeviceContext *aDeviceContext, ID3D11DepthStencilView *aDepthStencilView,
	float aRed, float aGreen, float aBlue, float aAlpha)
{
	float color[4]{ aRed, aGreen, aBlue, aAlpha };

	aDeviceContext->ClearRenderTargetView(myRenderTargetView, color);
	aDeviceContext->ClearDepthStencilView(aDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ID3D11ShaderResourceView* RenderTextureClass::GetShaderResourceView()
{
	return myShaderResourceView;
}
