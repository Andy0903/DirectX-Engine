#ifndef MODELCLASS_H
#define MODELCLASS_H

#include <d3d11.h>
#include <D3DX10math.h>
#include "TextureClass.h"
#include <fstream>;

class ModelClass
{
public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, char*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	//structure to represent the model format
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
		///D3DXVECTOR4 color;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadModel(char*);
	void ReleaseModel();
	
	ID3D11Buffer *myVertexBuffer;
	ID3D11Buffer *myIndexBuffer;
	int myVertexCount;
	int myIndexCount;

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();
	TextureClass *myTexture;

	//Hold the model data before it is placed in the vertex buffer.
	ModelType *myModel;
};

#endif