#include "TextureShaderClass.h"

TextureShaderClass::TextureShaderClass()
{
	myVertexShader = nullptr;
	myPixelShader = nullptr;
	myLayout = nullptr;
	myMatrixBuffer = nullptr;
	mySampleState = nullptr;
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass &aOther)
{
}

TextureShaderClass::~TextureShaderClass()
{
}

bool TextureShaderClass::Initialize(ID3D11Device *aDevice, HWND aHWND)
{
	bool result;
	result = InitializeShader(aDevice, aHWND, L"../Engine/Texture.vs", L"../Engine/Texture.ps");
	return result;
}

void TextureShaderClass::Shutdown()
{
	ShutdownShader();
}

bool TextureShaderClass::Render(ID3D11DeviceContext *aDeviceContext, int aIndexCount, D3DXMATRIX aWorldMatrix,
	D3DXMATRIX aViewMatrix, D3DXMATRIX aProjectionMatrix, ID3D11ShaderResourceView *aTexture)
{
	bool result;
	result = SetShaderParameters(aDeviceContext, aWorldMatrix, aViewMatrix, aProjectionMatrix, aTexture);
	if (result == false)
	{
		return false;
	}

	RenderShader(aDeviceContext, aIndexCount);
	return true;
}

bool TextureShaderClass::InitializeShader(ID3D11Device *aDevice, HWND aHWND, WCHAR *aVsFileName, WCHAR *aPsFileName)
{
	HRESULT result;
	ID3D10Blob *errorMessage;
	ID3D10Blob *vertexShaderBuffer;
	ID3D10Blob *pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	result = D3DX11CompileFromFile(aVsFileName, nullptr, nullptr, "TextureVertexShader", "vs_5_0",
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

	result = D3DX11CompileFromFile(aPsFileName, nullptr, nullptr, "TexturePixelShader", "ps_5_0",
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

	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = aDevice->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &myLayout);
	if (FAILED(result))
	{
		return false;
	}

	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = aDevice->CreateBuffer(&matrixBufferDesc, NULL, &myMatrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

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

	return true;
}

void TextureShaderClass::ShutdownShader()
{
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

void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob *aErrorMessage, HWND aHWND, WCHAR *aShaderFileName)
{
	char *compileErrors;
	unsigned long bufferSize;
	//unsigned long i;
	std::ofstream fOut;

	compileErrors = (char*)(aErrorMessage->GetBufferPointer());
	bufferSize = aErrorMessage->GetBufferSize();
	fOut.open("Shader-error.txt");
	for (int i = 0; i < bufferSize; i++)
	{
		fOut << compileErrors[i];
	}

	fOut.close();

	aErrorMessage->Release();
	aErrorMessage = nullptr;

	MessageBox(aHWND, L"Error compiling shader. Check Shader-error.txt for message.", aShaderFileName, MB_OK);
}

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext *aDeviceContext, D3DXMATRIX aWorldMatrix, D3DXMATRIX aViewMatrix,
	D3DXMATRIX aProjectionMatrix, ID3D11ShaderResourceView *aTexture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType *data;
	unsigned int bufferNumber;


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

	return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext *aDeviceContext, int aIndexCount)
{
	aDeviceContext->IASetInputLayout(myLayout);
	aDeviceContext->VSSetShader(myVertexShader, nullptr, 0);
	aDeviceContext->PSSetShader(myPixelShader, nullptr, 0);

	aDeviceContext->PSSetSamplers(0, 1, &mySampleState);
	aDeviceContext->DrawIndexed(aIndexCount, 0, 0);
}

