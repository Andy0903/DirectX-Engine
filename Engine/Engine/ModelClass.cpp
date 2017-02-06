#include "ModelClass.h"

ModelClass::ModelClass()
{
	myVertexBuffer = nullptr;
	myIndexBuffer = nullptr;
	myTexture = nullptr;
	myModel = nullptr;
}

ModelClass::ModelClass(const ModelClass& aOther)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* aDevice, char* aModelFileName, WCHAR* aTextureFileName)
{
	bool result;

	result = LoadModel(aModelFileName);
	if (result == false) { return result; }

	result = InitializeBuffers(aDevice);
	if (result == false) { return result; }

	result = LoadTexture(aDevice, aTextureFileName);
	return result;
}

void ModelClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
	ReleaseModel();
}

void ModelClass::Render(ID3D11DeviceContext* aDeviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(aDeviceContext);
}

int ModelClass::GetIndexCount()
{
	return myIndexCount;
}

//Drawing Triangle OUTDATES
bool ModelClass::InitializeBuffers(ID3D11Device* aDevice)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT result;

	//myVertexCount = 3;
	//myIndexCount = 3;

	vertices = new VertexType[myVertexCount];
	indices = new unsigned long[myIndexCount];

	for (int i = 0; i < myVertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(myModel[i].x, myModel[i].y, myModel[i].z);
		vertices[i].texture = D3DXVECTOR2(myModel[i].tu, myModel[i].tv);
		vertices[i].normal = D3DXVECTOR3(myModel[i].nx, myModel[i].ny, myModel[i].nz);

		indices[i] = i;
	}

	////vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
	////vertices[0].color = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	//
	////vertices[1].position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
	////vertices[1].color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	//
	////vertices[2].position = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	////vertices[2].color = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);

	//vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f); //Bottom left
	//vertices[0].texture = D3DXVECTOR2(0.0f, 1.0f);
	//vertices[0].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	//vertices[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f); // Top middle
	//vertices[1].texture = D3DXVECTOR2(0.5f, 0.0f);
	//vertices[1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	//vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f); //Bottom right
	//vertices[2].texture = D3DXVECTOR2(1.0f, 1.0f);
	//vertices[2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	//indices[0] = 0;
	//indices[1] = 1;
	//indices[2] = 2;

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * myVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = aDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
	if (FAILED(result)) { return false; }

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * myIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = aDevice->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);
	if (FAILED(result)) { return false; }

	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void ModelClass::ShutdownBuffers()
{
	if (myIndexBuffer)
	{
		myIndexBuffer->Release();
		myIndexBuffer = nullptr;
	}

	if (myVertexBuffer)
	{
		myVertexBuffer->Release();
		myVertexBuffer = nullptr;
	}
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* aDeviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	aDeviceContext->IASetVertexBuffers(0, 1, &myVertexBuffer, &stride, &offset);
	aDeviceContext->IASetIndexBuffer(myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	aDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return myTexture->GetTexture();
}

bool ModelClass::LoadTexture(ID3D11Device* aDevice, WCHAR* aFileName)
{
	bool result;
	myTexture = new TextureClass;
	if (!myTexture)
	{
		return false;
	}

	result = myTexture->Initialize(aDevice, aFileName);
	return result;
}

void ModelClass::ReleaseTexture()
{
	if (myTexture)
	{
		myTexture->Shutdown();
		delete myTexture;
		myTexture = nullptr;
	}
}

bool ModelClass::LoadModel(char* aFileName)
{
	std::ifstream fileIn;
	char input;

	fileIn.open(aFileName);
	if (fileIn.fail()) { return false; }

	//Read up to the value of vertex count in file.
	fileIn.get(input);
	while (input != ':')
	{
		fileIn.get(input);
	}

	fileIn >> myVertexCount;
	myIndexCount = myVertexCount;

	myModel = new ModelType[myVertexCount];
	
	//Read beginning of model data in file.
	fileIn.get(input);
	while (input != ':')
	{
		fileIn.get(input);
	}
	fileIn.get(input);			//Ugly way of skipping "human-specific" text in the data.
	fileIn.get(input);

	for (int i = 0; i < myVertexCount; i++)
	{
		fileIn >> myModel[i].x >> myModel[i].y >> myModel[i].z;
		fileIn >> myModel[i].tu >> myModel[i].tv;
		fileIn >> myModel[i].nx >> myModel[i].ny >> myModel[i].nz;
	}

	fileIn.close();
	return true;
}

void ModelClass::ReleaseModel()
{
	if (myModel)
	{
		delete[] myModel;
		myModel = nullptr;
	}
}