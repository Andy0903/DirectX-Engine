#ifndef RENDERTEXTURECLASS_H
#define RENDERTEXTURECLASS_H

#include <d3d11.h>

class RenderTextureClass
{
public:
	RenderTextureClass();
	RenderTextureClass(const RenderTextureClass&);
	~RenderTextureClass();

	bool Initialize(ID3D11Device*, int, int);
	void Shutdown();

	void SetRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*);
	void ClearRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*, float, float, float, float);
	ID3D11ShaderResourceView* GetShaderResourceView();

private:
	ID3D11Texture2D *myRenderTargetTexture;
	ID3D11RenderTargetView *myRenderTargetView;
	ID3D11ShaderResourceView *myShaderResourceView;
};

#endif