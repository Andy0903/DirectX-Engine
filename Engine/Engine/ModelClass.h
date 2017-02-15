#ifndef MODELCLASS_H
#define MODELCLASS_H

#include <d3d11.h>
#include <D3DX10math.h>
#include "TextureArrayClass.h"
#include <fstream>;

class ModelClass
{
public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, char*, WCHAR*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView **GetTextureArray();
	void SetPosition(float, float, float);
	void GetPosition(float&, float&, float&);

private:
	//structure to represent the model format
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
		///D3DXVECTOR4 color;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 tangent;
		D3DXVECTOR3 binormal;
	};

	//The following two structures will be used for calculating the tangent and binormal.
	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType aVertices[], VectorType& atangent, VectorType& aBinormal);
	void CalculateNormal(VectorType, VectorType, VectorType&);

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadModel(char*);
	void ReleaseModel();
	
	ID3D11Buffer *myVertexBuffer;
	ID3D11Buffer *myIndexBuffer;
	int myVertexCount;
	int myIndexCount;

	bool LoadTextures(ID3D11Device*, WCHAR*, WCHAR*);
	void ReleaseTextures();
	TextureArrayClass *myTextureArray;

	//Hold the model data before it is placed in the vertex buffer.
	ModelType *myModel;
	float m_positionX, m_positionY, m_positionZ;
};

#endif