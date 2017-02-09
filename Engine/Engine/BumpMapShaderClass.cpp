#include "BumpMapShaderClass.h"

BumpMapShaderClass::BumpMapShaderClass()
{
	myVertexShader = nullptr;
	myPixelShader = nullptr;
	myLayout = nullptr;
	myMatrixBuffer = nullptr;
	mySampleState = nullptr;
	myLightBuffer = nullptr;
}


BumpMapShaderClass::~BumpMapShaderClass()
{
}

bool BumpMapShaderClass::Initialize(ID3D11Device *aDevice, HWND aHwnd)
{
	return InitializeShader(aDevice, aHwnd, L"../Engine/Bumpmap.vs", L"../Engine/Bumpmap.ps");
}

void BumpMapShaderClass::Shutdown()
{
	ShutdownShader();
}

bool BumpMapShaderClass::Render(ID3D11DeviceContext *aDeviceContext, int aIndexCount, D3DXMATRIX aWorldMatrix, D3DXMATRIX aViewMatrix,
	D3DXMATRIX aProjectionMatrix, ID3D11ShaderResourceView **aTextureArray, D3DXVECTOR3 aLightDirection,
	D3DXVECTOR4 aDiffuseColor)
{
	bool result = SetShaderParameters(aDeviceContext, aWorldMatrix, aViewMatrix, aProjectionMatrix, aTextureArray, aLightDirection, aDiffuseColor);
	if (result == false) { return false; }

	RenderShader(aDeviceContext, aIndexCount);

	return true;
}

bool BumpMapShaderClass::InitializeShader(ID3D11Device* aDevice, HWND aHwnd, WCHAR* aVsFilename, WCHAR* aPsFilename)
{
	HRESULT result;
	ID3D10Blob *errorMessage = nullptr;
	ID3D10Blob *vertexShaderBuffer = nullptr;
	ID3D10Blob *pixelShaderBuffer = nullptr;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[5];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

	//VS load
	result = D3DX11CompileFromFile(aVsFilename, nullptr, nullptr, "BumpMapVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, nullptr, &vertexShaderBuffer, &errorMessage, nullptr);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, aHwnd, aVsFilename);
		}
		else
		{
			MessageBox(aHwnd, aVsFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	//PS load
	result = D3DX11CompileFromFile(aPsFilename, nullptr, nullptr, "BumpMapPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
		0, nullptr, &pixelShaderBuffer, &errorMessage, nullptr);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, aHwnd, aPsFilename);
		}
		else
		{
			MessageBox(aHwnd, aPsFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	result = aDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr,
		&myVertexShader);
	if (FAILED(result)) { return false; }

	result = aDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr,
		&myPixelShader);
	if (FAILED(result)) { return false; }

	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = aDevice->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &myLayout);
	if (FAILED(result)) { return false; }

	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	result = aDevice->CreateBuffer(&matrixBufferDesc, nullptr, &myMatrixBuffer);
	if (FAILED(result)) { return false; }

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	result = aDevice->CreateSamplerState(&samplerDesc, &mySampleState);
	if (FAILED(result)) { return false; }

	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	result = aDevice->CreateBuffer(&lightBufferDesc, nullptr, &myLightBuffer);
	if (FAILED(result)) { return false; }

	return true;
}

void BumpMapShaderClass::ShutdownShader()
{
	if (myLightBuffer)
	{
		myLightBuffer->Release();
		myLightBuffer = nullptr;
	}

	if (mySampleState)
	{
		mySampleState->Release();
		mySampleState = nullptr;
	}

	if (myMatrixBuffer)
	{
		myMatrixBuffer->Release();
		myMatrixBuffer = nullptr;
	}

	if (myLayout)
	{
		myLayout->Release();
		myLayout = nullptr;
	}

	if (myPixelShader)
	{
		myPixelShader->Release();
		myPixelShader = nullptr;
	}

	if (myVertexShader)
	{
		myVertexShader->Release();
		myVertexShader = nullptr;
	}
}

void BumpMapShaderClass::OutputShaderErrorMessage(ID3D10Blob *aErrorMessage, HWND aHwnd, WCHAR *aShaderFilename)
{
	char *compileErrors;
	unsigned long bufferSize;
	std::ofstream fout;

	compileErrors = (char*)(aErrorMessage->GetBufferPointer());

	bufferSize = aErrorMessage->GetBufferSize();

	fout.open("shader-error.txt");

	for (int i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	fout.close();

	aErrorMessage->Release();
	aErrorMessage = nullptr;

	MessageBox(aHwnd, L"Error compiling shader.  Check shader-error.txt for message.", aShaderFilename, MB_OK);
}

bool BumpMapShaderClass::SetShaderParameters(ID3D11DeviceContext *aDeviceContext, D3DXMATRIX aWorldMatrix,
	D3DXMATRIX aViewMatrix, D3DXMATRIX aProjectionMatrix,
	ID3D11ShaderResourceView ** aTextureArray, D3DXVECTOR3 aLightDirection,
	D3DXVECTOR4 aDiffuseColor)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType *matrixBuffer;
	unsigned int bufferNumber;
	LightBufferType *lightBuffer;

	//Transpose to prep for shader.
	D3DXMatrixTranspose(&aWorldMatrix, &aWorldMatrix);
	D3DXMatrixTranspose(&aViewMatrix, &aViewMatrix);
	D3DXMatrixTranspose(&aProjectionMatrix, &aProjectionMatrix);

	result = aDeviceContext->Map(myMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	matrixBuffer = (MatrixBufferType*)mappedResource.pData;
	matrixBuffer->world = aWorldMatrix;
	matrixBuffer->view = aViewMatrix;
	matrixBuffer->projection = aProjectionMatrix;

	aDeviceContext->Unmap(myMatrixBuffer, 0);
	bufferNumber = 0;
	aDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &myMatrixBuffer);

	aDeviceContext->PSSetShaderResources(0, 2, aTextureArray);

	result = aDeviceContext->Map(myLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	lightBuffer = (LightBufferType*)mappedResource.pData;
	lightBuffer->diffuseColor = aDiffuseColor;
	lightBuffer->lightDirection = aLightDirection;

	aDeviceContext->Unmap(myLightBuffer, 0);
	bufferNumber = 0;
	aDeviceContext->PSSetConstantBuffers(bufferNumber, 1, &myLightBuffer);

	return true;
}

void BumpMapShaderClass::RenderShader(ID3D11DeviceContext *aDeviceContext, int aIndexCount)
{
	aDeviceContext->IASetInputLayout(myLayout);

	aDeviceContext->VSSetShader(myVertexShader, nullptr, 0);
	aDeviceContext->PSSetShader(myPixelShader, nullptr, 0);

	aDeviceContext->PSSetSamplers(0, 1, &mySampleState);

	aDeviceContext->DrawIndexed(aIndexCount, 0, 0);
}