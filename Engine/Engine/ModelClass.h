#ifndef MODELCLASS_H
#define MODELCLASS_H

#include <d3d11.h>
#include <D3DX10math.h>
#include "TextureClass.h"

class ModelClass
{
public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		//D3DXVECTOR4 color;
		D3DXVECTOR2 texture;
	};

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	
	ID3D11Buffer *myVertexBuffer;
	ID3D11Buffer *myIndexBuffer;
	int myVertexCount;
	int myIndexCount;

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();
	TextureClass* myTexture;
};

#endif