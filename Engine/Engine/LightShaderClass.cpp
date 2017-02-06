#include "LightShaderClass.h"

LightShaderClass::LightShaderClass()
{
	myVertexShader = nullptr;
	myPixelShader = nullptr;
	myLayout = nullptr;
	mySampleState = nullptr;
	myMatrixBuffer = nullptr;
	myLightBuffer = nullptr;
}

LightShaderClass::LightShaderClass(const LightShaderClass &aOther)
{
}

LightShaderClass::~LightShaderClass()
{
}

bool LightShaderClass::Initialize(ID3D11Device *aDevice, HWND aHWND)
{
	return InitializeShader(aDevice, aHWND, L"../Engine/Light.vs", L"../Engine/Light.ps");
}

void LightShaderClass::Shutdown()
{
	ShutdownShader();
}

bool LightShaderClass::Render(ID3D11DeviceContext *aDeviceContext, int aIndexCount, D3DXMATRIX aWorldMatrix,
	D3DXMATRIX aViewMatrix, D3DXMATRIX aProjectionMatrix, ID3D11ShaderResourceView* aTexture, D3DXVECTOR3 aLightDirection,
	D3DXVECTOR4 aAmbientColor, D3DXVECTOR4 aDiffuseColor)
{
	bool result;

	result = SetShaderParameters(aDeviceContext, aWorldMatrix, aViewMatrix,
		aProjectionMatrix, aTexture, aLightDirection, aAmbientColor, aDiffuseColor);
	if (result == false)
	{
		return false;
	}

	RenderShader(aDeviceContext, aIndexCount);
	return true;
}

bool LightShaderClass::InitializeShader(ID3D11Device *aDevice, HWND aHWND, WCHAR *aVsFileName, WCHAR *aPsFileName)
{
	HRESULT result;
	ID3D10Blob *errorMessage;
	ID3D10Blob *vertexShaderBuffer;
	ID3D10Blob *pixelShaderBuffer;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;

	D3D11_BUFFER_DESC lightBufferDesc;

	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	result = D3DX11CompileFromFile(aVsFileName, nullptr, nullptr, "LightVertexShader", "vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &vertexShaderBuffer, &errorMessage, nullptr);

	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, aHWND, aVsFileName);
		}
		else
		{
			MessageBox(aHWND, aVsFileName, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	result = D3DX11CompileFromFile(aPsFileName, nullptr, nullptr, "LightPixelShader", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &pixelShaderBuffer, &errorMessage, nullptr);

	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, aHWND, aPsFileName);
		}
		else
		{
			MessageBox(aHWND, aPsFileName, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	result = aDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), nullptr, &myVertexShader);
	if (FAILED(result))
	{
		return false;
	}

	result = aDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
		pixelShaderBuffer->GetBufferSize(), nullptr, &myPixelShader);
	if (FAILED(result))
	{
		return false;
	}

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

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = aDevice->CreateInputLayout(polygonLayout, numElements,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &myLayout);
	if (FAILED(result))
	{
		return false;
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

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
	if (FAILED(result))
	{
		return false;
	}

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = aDevice->CreateBuffer(&matrixBufferDesc, nullptr, &myMatrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	result = aDevice->CreateBuffer(&lightBufferDesc, nullptr, &myLightBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void LightShaderClass::ShutdownShader()
{
	if (myLightBuffer)
	{
		myLightBuffer->Release();
		myLightBuffer = 0;
	}

	if (myMatrixBuffer)
	{
		myMatrixBuffer->Release();
		myMatrixBuffer = 0;
	}

	if (mySampleState)
	{
		mySampleState->Release();
		mySampleState = 0;
	}

	if (myLayout)
	{
		myLayout->Release();
		myLayout = 0;
	}

	if (myPixelShader)
	{
		myPixelShader->Release();
		myPixelShader = 0;
	}

	if (myVertexShader)
	{
		myVertexShader->Release();
		myVertexShader = 0;
	}
}

void LightShaderClass::OutputShaderErrorMessage(ID3D10Blob *aErrorMessage, HWND aHWND, WCHAR* aShaderFileName)
{
	char* compileErrors;
	unsigned long bufferSize;
	std::ofstream fOut;

	compileErrors = (char*)(aErrorMessage->GetBufferPointer());
	bufferSize = aErrorMessage->GetBufferSize();
	fOut.open("shader-error.txt");

	for (int i = 0; i < bufferSize; i++)
	{
		fOut << compileErrors[i];
	}
	fOut.close();

	aErrorMessage->Release();
	aErrorMessage = nullptr;

	MessageBox(aHWND, L"Error compiling shader.  Check shader-error.txt for message.", aShaderFileName, MB_OK);
}

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* aDeviceContext, D3DXMATRIX aWorldMatrix,
	D3DXMATRIX aViewMatrix, D3DXMATRIX aProjectionMatrix, ID3D11ShaderResourceView* aTexture,
	D3DXVECTOR3 aLightDirection, D3DXVECTOR4 aAmbientColor, D3DXVECTOR4 aDiffuseColor)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* data;
	LightBufferType* lightData;

	D3DXMatrixTranspose(&aWorldMatrix, &aWorldMatrix);
	D3DXMatrixTranspose(&aViewMatrix, &aViewMatrix);
	D3DXMatrixTranspose(&aProjectionMatrix, &aProjectionMatrix);

	result = aDeviceContext->Map(myMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	data = (MatrixBufferType*)mappedResource.pData;
	data->world = aWorldMatrix;
	data->view = aViewMatrix;
	data->projection = aProjectionMatrix;

	aDeviceContext->Unmap(myMatrixBuffer, 0);
	bufferNumber = 0;

	aDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &myMatrixBuffer);
	aDeviceContext->PSSetShaderResources(0, 1, &aTexture);

	result = aDeviceContext->Map(myLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	lightData = (LightBufferType*)mappedResource.pData;
	lightData->ambientColor = aAmbientColor;
	lightData->diffuseColor = aDiffuseColor;
	lightData->lightDirection = aLightDirection;
	lightData->padding = 0.0f;

	aDeviceContext->Unmap(myLightBuffer, 0);
	bufferNumber = 0;

	aDeviceContext->PSSetConstantBuffers(bufferNumber, 1, &myLightBuffer);
	return true;
}

void LightShaderClass::RenderShader(ID3D11DeviceContext* aDeviceContext, int aIndexCount)
{
	aDeviceContext->IASetInputLayout(myLayout);
	aDeviceContext->VSSetShader(myVertexShader, nullptr, 0);
	aDeviceContext->PSSetShader(myPixelShader, nullptr, 0);

	aDeviceContext->PSSetSamplers(0, 1, &mySampleState);
	aDeviceContext->DrawIndexed(aIndexCount, 0, 0);
}