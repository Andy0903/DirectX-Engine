#include "BitMapClass.h"

BitmapClass::BitmapClass()
{
	myVertexBuffer = nullptr;
	myIndexBuffer = nullptr;
	myTexture = nullptr;
}

BitmapClass::BitmapClass(const BitmapClass&)
{
}

BitmapClass::~BitmapClass()
{
}

bool BitmapClass::Initialize(ID3D11Device *aDevice, int aScreenWidth, int aScreenHeight, WCHAR *aTextureFilename, int aBitmapWidth, int aBitmapHeight)
{
	bool result;

	// Store the screen size.
	myScreenWidth = aScreenWidth;
	myScreenHeight = aScreenHeight;

	// Store the size in pixels that this bitmap should be rendered at.
	myBitmapWidth = aBitmapWidth;
	myBitmapHeight = aBitmapHeight;

	// Initialize the previous rendering position to negative one.
	myPreviousPosX = -1;
	myPreviousPosY = -1;

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(aDevice);
	if (!result)
	{
		return false;
	}

	// Load the texture for this model.
	return LoadTexture(aDevice, aTextureFilename);
}

void BitmapClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
}

bool BitmapClass::Render(ID3D11DeviceContext *aDeviceContext, int aPositionX, int aPositionY)
{
	bool result;

	// Re-build the dynamic vertex buffer for rendering to possibly a different location on the screen.
	result = UpdateBuffers(aDeviceContext, aPositionX, aPositionY);
	if (!result) { return false; }

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(aDeviceContext);

	return true;
}

int BitmapClass::GetIndexCount()
{
	return myIndexCount;
}

ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
	return myTexture->GetTexture();
}

bool BitmapClass::InitializeBuffers(ID3D11Device *aDevice)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set the number of vertices in the vertex array.
	myVertexCount = 6;

	// Set the number of indices in the index array.
	myIndexCount = myVertexCount;

	// Create the vertex array.
	vertices = new VertexType[myVertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[myIndexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * myVertexCount));

	// Load the index array with data.
	for (int i = 0; i < myIndexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the DYNAMIC vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * myVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = aDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
	if (FAILED(result)) { return false; }

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * myIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = aDevice->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);
	if (FAILED(result)) { return false; }

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;

	return true;
}

void BitmapClass::ShutdownBuffers()
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

bool BitmapClass::UpdateBuffers(ID3D11DeviceContext *aDeviceContext, int aPositionX, int aPositionY)
{
	float left, right, top, bottom;
	VertexType *vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType *verticesPtr;
	HRESULT result;

	if ((aPositionX == myPreviousPosX) && (aPositionY == myPreviousPosY))
	{
		return true;
	}

	myPreviousPosX = aPositionX;
	myPreviousPosY = aPositionY;

	// Calculate the screen coordinates of the left side of the bitmap.
	left = (float)((myScreenWidth / 2) * -1) + (float)aPositionX;
	// Calculate the screen coordinates of the right side of the bitmap.
	right = left + (float)myBitmapWidth;
	// Calculate the screen coordinates of the top of the bitmap.
	top = (float)(myScreenHeight / 2) - (float)aPositionY;
	// Calculate the screen coordinates of the bottom of the bitmap.
	bottom = top - (float)myBitmapHeight;


	vertices = new VertexType[myVertexCount];
	// Load the vertex array with data.
	// First triangle.
	vertices[0].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

	// Second triangle.
	vertices[3].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[4].position = D3DXVECTOR3(right, top, 0.0f);  // Top right.
	vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);

	vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);

	result = aDeviceContext->Map(myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	verticesPtr = (VertexType*)mappedResource.pData;
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * myVertexCount));
	aDeviceContext->Unmap(myVertexBuffer, 0);

	delete[] vertices;
	vertices = 0;

	return true;
}

void BitmapClass::RenderBuffers(ID3D11DeviceContext *aDeviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	aDeviceContext->IASetVertexBuffers(0, 1, &myVertexBuffer, &stride, &offset);
	aDeviceContext->IASetIndexBuffer(myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	aDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool BitmapClass::LoadTexture(ID3D11Device *aDevice, WCHAR *aFilename)
{
	bool result;
	myTexture = new TextureClass;
	return myTexture->Initialize(aDevice, aFilename);
}

void BitmapClass::ReleaseTexture()
{
	if (myTexture)
	{
		myTexture->Shutdown();
		delete myTexture;
		myTexture = nullptr;
	}
}