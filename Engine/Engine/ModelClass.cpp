#include "ModelClass.h"

ModelClass::ModelClass()
{
	myVertexBuffer = nullptr;
	myIndexBuffer = nullptr;
	myTextureArray = nullptr;
	myModel = nullptr;
}

ModelClass::ModelClass(const ModelClass& aOther)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* aDevice, char* aModelFileName, WCHAR* aTexFile1, WCHAR* aTexFile2)
{
	bool result;

	result = LoadModel(aModelFileName);
	if (result == false) { return result; }
	CalculateModelVectors();

	result = InitializeBuffers(aDevice);
	if (result == false) { return result; }

	result = LoadTextures(aDevice, aTexFile1, aTexFile2);
	return result;
}

void ModelClass::Shutdown()
{
	ReleaseTextures();
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

	vertices = new VertexType[myVertexCount];
	indices = new unsigned long[myIndexCount];

	for (int i = 0; i < myVertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(myModel[i].x, myModel[i].y, myModel[i].z);
		vertices[i].texture = D3DXVECTOR2(myModel[i].tu, myModel[i].tv);
		vertices[i].normal = D3DXVECTOR3(myModel[i].nx, myModel[i].ny, myModel[i].nz);
		vertices[i].tangent = D3DXVECTOR3(myModel[i].tx, myModel[i].ty, myModel[i].tz);
		vertices[i].binormal = D3DXVECTOR3(myModel[i].bx, myModel[i].by, myModel[i].bz);

		indices[i] = i;
	}

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

ID3D11ShaderResourceView** ModelClass::GetTextureArray()
{
	return myTextureArray->GetTextureArray();
}

bool ModelClass::LoadTextures(ID3D11Device *aDevice, WCHAR *aTexFile1, WCHAR *aTexFile2)
{
	bool result;
	myTextureArray = new TextureArrayClass;
	if (!myTextureArray)
	{
		return false;
	}

	result = myTextureArray->Initialize(aDevice, aTexFile1, aTexFile2);
	return result;
}

void ModelClass::CalculateModelVectors()
{
	///CalculateModelVectors generates the tangent and binormal for the model as well as a recalculated normal vector.
	///To start it calculates how many faces (triangles) are in the model.
	///Then for each of those triangles it gets the three vertices and uses that to calculate the tangent, binormal, and normal.
	///After calculating those three normal vectors it then saves them back into the model structure.

	int faceCount, index;
	TempVertexType vertices[3];
	VectorType tangent, binormal, normal;

	const int NumberOfVertsInaTriangle = 3;
	faceCount = myVertexCount / NumberOfVertsInaTriangle;
	index = 0;

	for (int i = 0; i < faceCount; i++)
	{
		// Get the three vertices for this face from the model.
		for (int k = 0; k < 3; k++)
		{
			vertices[k].x = myModel[k].x;
			vertices[k].y = myModel[k].y;
			vertices[k].z = myModel[k].z;
			vertices[k].tu = myModel[k].tu;
			vertices[k].tv = myModel[k].tv;
			vertices[k].nx = myModel[k].nx;
			vertices[k].ny = myModel[k].ny;
			vertices[k].nz = myModel[k].nz;
		}

		CalculateTangentBinormal(vertices, tangent, binormal);
		CalculateNormal(tangent, binormal, normal);

		// Store the normal, tangent, and binormal for this face back in the model structure.
		for (int l = 0; l < 3; l++)
		{
			vertices[l].x = myModel[l].x;
			vertices[l].y = myModel[l].y;
			vertices[l].z = myModel[l].z;
			vertices[l].tu = myModel[l].tu;
			vertices[l].tv = myModel[l].tv;
			vertices[l].nx = myModel[l].nx;
			vertices[l].ny = myModel[l].ny;
			vertices[l].nz = myModel[l].nz;
		}
	}
}

void ModelClass::CalculateTangentBinormal(TempVertexType aVertices[], VectorType& aTangent, VectorType& aBinormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float denomenator;
	float length;

	//Calc the vectors of the face
	vector1[0] = aVertices[1].x - aVertices[0].x;
	vector1[1] = aVertices[1].y - aVertices[0].y;
	vector1[2] = aVertices[1].z - aVertices[0].z;

	vector2[0] = aVertices[2].x - aVertices[0].x;
	vector2[1] = aVertices[2].y - aVertices[0].y;
	vector2[2] = aVertices[2].z - aVertices[0].z;

	//Calc tu and tv tex-space vectors.
	tuVector[0] = aVertices[1].tu - aVertices[0].tu;
	tvVector[0] = aVertices[1].tv - aVertices[0].tv;

	tuVector[1] = aVertices[2].tu - aVertices[0].tu;
	tvVector[1] = aVertices[2].tv - aVertices[0].tv;

	//Calc denominator of the tangent/binormal equation.
	denomenator = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	//Calc the cross product and multiply the coefficient to get the tangent and binormal.
	aTangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * denomenator;
	aTangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * denomenator;
	aTangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * denomenator;

	aBinormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * denomenator;
	aBinormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * denomenator;
	aBinormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * denomenator;

	//Calc length of tangent.
	length = sqrt((aTangent.x * aTangent.x) + (aTangent.y * aTangent.y) + (aTangent.z *aTangent.z));

	//Normalize tangent
	aTangent.x = aTangent.x / length;
	aTangent.y = aTangent.y / length;
	aTangent.z = aTangent.z / length;

	//Calc length of binormal.
	length = sqrt((aBinormal.x * aBinormal.x) + (aBinormal.y * aBinormal.y) + (aBinormal.z *aBinormal.z));

	//Normalize binormal
	aBinormal.x = aBinormal.x / length;
	aBinormal.y = aBinormal.y / length;
	aBinormal.z = aBinormal.z / length;
}

void ModelClass::CalculateNormal(VectorType aTangent, VectorType aBinormal, VectorType& aNormal)
{
	float length;

	//Calc the cross product of the tangent and binormal to get normal. //Handregeln
	aNormal.x = (aTangent.y * aBinormal.z) - (aTangent.z * aBinormal.y);
	aNormal.y = (aTangent.z * aBinormal.x) - (aTangent.x * aBinormal.z);
	aNormal.z = (aTangent.x * aBinormal.y) - (aTangent.y * aBinormal.x);

	//Calc length of normal
	length = sqrt((aNormal.x * aNormal.x) + (aNormal.y * aNormal.y) + (aNormal.z * aNormal.z));

	//Normalize normal
	aNormal.x = aNormal.x / length;
	aNormal.y = aNormal.y / length;
	aNormal.z = aNormal.z / length;
}

void ModelClass::ReleaseTextures()
{
	if (myTextureArray)
	{
		myTextureArray->Shutdown();
		delete myTextureArray;
		myTextureArray = nullptr;
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

void ModelClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
}

void ModelClass::GetPosition(float& x, float& y, float& z)
{
	x = m_positionX;
	y = m_positionY;
	z = m_positionZ;
}