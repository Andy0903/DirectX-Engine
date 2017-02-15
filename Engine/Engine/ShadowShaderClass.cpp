#include "ShadowShaderClass.h"

ShadowShaderClass::ShadowShaderClass()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_sampleStateWrap = nullptr;
	m_sampleStateClamp = nullptr;
	m_matrixBuffer = nullptr;
	m_lightBuffer = nullptr;
	m_lightBuffer2 = nullptr;
}

ShadowShaderClass::ShadowShaderClass(const ShadowShaderClass &aOther)
{
}

ShadowShaderClass::~ShadowShaderClass()
{
}

bool ShadowShaderClass::Initialize(ID3D11Device *aDevice, HWND aHwnd)
{
	return InitializeShader(aDevice, aHwnd, L"../Engine/shadow.vs", L"../Engine/shadow.ps");
}

void ShadowShaderClass::Shutdown()
{
	ShutdownShader();
}

bool ShadowShaderClass::Render(ID3D11DeviceContext *aDeviceContext, int aIndexCount, D3DXMATRIX aWorldMatrix, D3DXMATRIX aViewMatrix,
	D3DXMATRIX aProjectionMatrix, D3DXMATRIX aLightViewMatrix, D3DXMATRIX aLightProjectionMatrix,
	ID3D11ShaderResourceView *aTexture, ID3D11ShaderResourceView *aDepthMapTexture, D3DXVECTOR3 aLightPosition,
	D3DXVECTOR4 aAmbientColor, D3DXVECTOR4 aDiffuseColor)
{
	bool result;
	result = SetShaderParameters(aDeviceContext, aWorldMatrix, aViewMatrix, aProjectionMatrix, aLightViewMatrix, aLightProjectionMatrix, aTexture,
		aDepthMapTexture, aLightPosition, aAmbientColor, aDiffuseColor);
	if (!result) { return false; }

	RenderShader(aDeviceContext, aIndexCount);
	return true;
}

bool ShadowShaderClass::InitializeShader(ID3D11Device *aDevice, HWND aHwnd, WCHAR *aVsFilename, WCHAR *aPsFilename)
{
	HRESULT result;
	ID3D10Blob *errorMessage = nullptr;
	ID3D10Blob *vertexShaderBuffer = nullptr;
	ID3D10Blob *pixelShaderBuffer = nullptr;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc2;

	// Compile the vertex shader code.
	result = D3DX11CompileFromFile(aVsFilename, nullptr, nullptr, "ShadowVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr,
		&vertexShaderBuffer, &errorMessage, nullptr);
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

	// Compile the pixel shader code.
	result = D3DX11CompileFromFile(aPsFilename, nullptr, nullptr, "ShadowPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr,
		&pixelShaderBuffer, &errorMessage, nullptr);
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

	// Create the vertex shader from the buffer.
	result = aDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &m_vertexShader);
	if (FAILED(result)) { return false; }
	// Create the pixel shader from the buffer.
	result = aDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &m_pixelShader);
	if (FAILED(result)) { return false; }

	// Create the vertex input layout description.
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
	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
	// Create the vertex input layout.
	result = aDevice->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)) { return false; }

	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	// Create a wrap texture sampler state description.
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
	result = aDevice->CreateSamplerState(&samplerDesc, &m_sampleStateWrap);
	if (FAILED(result)) { return false; }

	// Create a clamp texture sampler state description.
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	result = aDevice->CreateSamplerState(&samplerDesc, &m_sampleStateClamp);
	if (FAILED(result)) { return false; }

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	result = aDevice->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
	if (FAILED(result)) { return false; }

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	result = aDevice->CreateBuffer(&lightBufferDesc, nullptr, &m_lightBuffer);
	if (FAILED(result)) { return false; }

	// Setup the description of the light dynamic constant buffer that is in the vertex shader.
	lightBufferDesc2.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc2.ByteWidth = sizeof(LightBufferType2);
	lightBufferDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc2.MiscFlags = 0;
	lightBufferDesc2.StructureByteStride = 0;
	result = aDevice->CreateBuffer(&lightBufferDesc2, nullptr, &m_lightBuffer2);
	if (FAILED(result)) { return false; }

	return true;
}

void ShadowShaderClass::ShutdownShader()
{
	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = nullptr;
	}

	if (m_lightBuffer2)
	{
		m_lightBuffer2->Release();
		m_lightBuffer2 = nullptr;
	}

	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	if (m_sampleStateWrap)
	{
		m_sampleStateWrap->Release();
		m_sampleStateWrap = nullptr;
	}
	if (m_sampleStateClamp)
	{
		m_sampleStateClamp->Release();
		m_sampleStateClamp = nullptr;
	}
	if (m_layout)
	{
		m_layout->Release();
		m_layout = nullptr;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}
}

void ShadowShaderClass::OutputShaderErrorMessage(ID3D10Blob *aErrorMessage, HWND aHwnd, WCHAR *aShaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	std::ofstream fout;

	compileErrors = (char*)(aErrorMessage->GetBufferPointer());
	bufferSize = aErrorMessage->GetBufferSize();
	fout.open("shader-error.txt");
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}
	fout.close();

	aErrorMessage->Release();
	aErrorMessage = nullptr;

	MessageBox(aHwnd, L"Error compiling shader.  Check shader-error.txt for message.", aShaderFilename, MB_OK);
}

bool ShadowShaderClass::SetShaderParameters(ID3D11DeviceContext *aDeviceContext, D3DXMATRIX aWorldMatrix, D3DXMATRIX aViewMatrix,
	D3DXMATRIX aProjectionMatrix, D3DXMATRIX aLightViewMatrix, D3DXMATRIX aLightProjectionMatrix,
	ID3D11ShaderResourceView *aTexture, ID3D11ShaderResourceView *aDepthMapTexture, D3DXVECTOR3 aLightPosition,
	D3DXVECTOR4 aAmbientColor, D3DXVECTOR4 aDiffuseColor)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* matrixBufferT;
	LightBufferType* lightBufferT;
	LightBufferType2* lightBufferT2;


	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&aWorldMatrix, &aWorldMatrix);
	D3DXMatrixTranspose(&aViewMatrix, &aViewMatrix);
	D3DXMatrixTranspose(&aProjectionMatrix, &aProjectionMatrix);
	D3DXMatrixTranspose(&aLightViewMatrix, &aLightViewMatrix);
	D3DXMatrixTranspose(&aLightProjectionMatrix, &aLightProjectionMatrix);

	result = aDeviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }
	matrixBufferT = (MatrixBufferType*)mappedResource.pData;
	matrixBufferT->world = aWorldMatrix;
	matrixBufferT->view = aViewMatrix;
	matrixBufferT->projection = aProjectionMatrix;
	matrixBufferT->lightView = aLightViewMatrix;
	matrixBufferT->lightProjection = aLightProjectionMatrix;
	aDeviceContext->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;
	aDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	aDeviceContext->PSSetShaderResources(0, 1, &aTexture);
	aDeviceContext->PSSetShaderResources(1, 1, &aDepthMapTexture);

	result = aDeviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }
	lightBufferT = (LightBufferType*)mappedResource.pData;
	lightBufferT->ambientColor = aAmbientColor;
	lightBufferT->diffuseColor = aDiffuseColor;
	aDeviceContext->Unmap(m_lightBuffer, 0);

	bufferNumber = 0;
	aDeviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	result = aDeviceContext->Map(m_lightBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }
	lightBufferT2 = (LightBufferType2*)mappedResource.pData;
	lightBufferT2->lightPosition = aLightPosition;
	lightBufferT2->padding = 0.0f;
	aDeviceContext->Unmap(m_lightBuffer2, 0);

	bufferNumber = 1;
	aDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer2);

	return true;
}

void ShadowShaderClass::RenderShader(ID3D11DeviceContext *aDeviceContext, int aIndexCount)
{
	aDeviceContext->IASetInputLayout(m_layout);
	aDeviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	aDeviceContext->PSSetShader(m_pixelShader, nullptr, 0);
	aDeviceContext->PSSetSamplers(0, 1, &m_sampleStateClamp);
	aDeviceContext->PSSetSamplers(1, 1, &m_sampleStateWrap);
	aDeviceContext->DrawIndexed(aIndexCount, 0, 0);
}