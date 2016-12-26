#ifndef LIGHTSHADERCLASS_H
#define LIGHTSHADERCLASS_H

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>

class LightShaderClass
{
public:
	LightShaderClass();
	LightShaderClass(const LightShaderClass&);
	~LightShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX,
		ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4);

private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float padding;
	};

	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX,
		ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4);
	void RenderShader(ID3D11DeviceContext*, int);

	ID3D11VertexShader *myVertexShader;
	ID3D11PixelShader *myPixelShader;
	ID3D11InputLayout *myLayout;
	ID3D11SamplerState *mySampleState;
	ID3D11Buffer *myMatrixBuffer;
	ID3D11Buffer *myLightBuffer;
};

#endif