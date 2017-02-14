#include "ReflectionShaderClass.h"

ReflectionShaderClass::ReflectionShaderClass()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_sampleState = nullptr;
	m_reflectionBuffer = nullptr;
}

ReflectionShaderClass::ReflectionShaderClass(const ReflectionShaderClass &aOther)
{
}

ReflectionShaderClass::~ReflectionShaderClass()
{
}

bool ReflectionShaderClass::Initialize(ID3D11Device *aDevice, HWND aHwnd)
{
	return InitializeShader(aDevice, aHwnd, L"../Engine/reflection.vs", L"../Engine/reflection.ps");
}

void ReflectionShaderClass::Shutdown()
{
	ShutdownShader();
}

bool ReflectionShaderClass::Render(ID3D11DeviceContext *aDeviceContext, int aIndexCount, D3DXMATRIX aWorldMatrix,
	D3DXMATRIX aViewMatrix, D3DXMATRIX aProjectionMatrix, ID3D11ShaderResourceView *aTexture,
	ID3D11ShaderResourceView *aReflectionTexture, D3DXMATRIX aReflectionMatrix)
{
	bool result;
	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(aDeviceContext, aWorldMatrix, aViewMatrix,
		aProjectionMatrix, aTexture, aReflectionTexture, aReflectionMatrix);
	if (!result) { return false; }

	// Now render the prepared buffers with the shader.
	RenderShader(aDeviceContext, aIndexCount);

	return true;
}


bool ReflectionShaderClass::InitializeShader(ID3D11Device *aDevice, HWND aHwnd, WCHAR *aVsFilename, WCHAR *aPsFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC reflectionBufferDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	// Compile the vertex shader code.
	result = D3DX11CompileFromFile(aVsFilename, nullptr, nullptr, "ReflectionVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
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

	//Compile the pixel shader code.
	result = D3DX11CompileFromFile(aPsFilename, nullptr, nullptr, "ReflectionPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
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

	// Create the vertex shader from the buffer.
	result = aDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr,
		&m_vertexShader);
	if (FAILED(result)) { return false; }

	// Create the vertex shader from the buffer.
	result = aDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr,
		&m_pixelShader);
	if (FAILED(result)) { return false; }

	// Create the vertex input layout description.
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

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = aDevice->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)) { return false; }

	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	// Setup the description of the matrix dynamic constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = aDevice->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
	if (FAILED(result)) { return false; }

	// Create a texture sampler state description.
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

	// Create the texture sampler state.
	result = aDevice->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result)) { return false; }

	// Setup the description of the reflection dynamic constant buffer that is in the vertex shader.
	reflectionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	reflectionBufferDesc.ByteWidth = sizeof(ReflectionBufferType);
	reflectionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	reflectionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	reflectionBufferDesc.MiscFlags = 0;
	reflectionBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = aDevice->CreateBuffer(&reflectionBufferDesc, NULL, &m_reflectionBuffer);
	if (FAILED(result)) { return false; }

	return true;
}

void ReflectionShaderClass::ShutdownShader()
{
	if (m_reflectionBuffer)
	{
		m_reflectionBuffer->Release();
		m_reflectionBuffer = nullptr;
	}

	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = nullptr;
	}

	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
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

void ReflectionShaderClass::OutputShaderErrorMessage(ID3D10Blob* aErrorMessage, HWND aHwnd, WCHAR *aShaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize;
	std::ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(aErrorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = aErrorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (int i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	fout.close();

	// Release the error message.
	aErrorMessage->Release();
	aErrorMessage = nullptr;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(aHwnd, L"Error compiling shader.  Check shader-error.txt for message.", aShaderFilename, MB_OK);
}

bool ReflectionShaderClass::SetShaderParameters(ID3D11DeviceContext *aDeviceContext, D3DXMATRIX aWorldMatrix, D3DXMATRIX aViewMatrix,
	D3DXMATRIX aProjectionMatrix, ID3D11ShaderResourceView *aTexture,
	ID3D11ShaderResourceView *aReflectionTexture, D3DXMATRIX aReflectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* matrixBufferType;
	unsigned int bufferNumber;
	ReflectionBufferType* reflectionBufferType;

	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&aWorldMatrix, &aWorldMatrix);
	D3DXMatrixTranspose(&aViewMatrix, &aViewMatrix);
	D3DXMatrixTranspose(&aProjectionMatrix, &aProjectionMatrix);

	// Transpose the relfection matrix to prepare it for the shader.
	D3DXMatrixTranspose(&aReflectionMatrix, &aReflectionMatrix);

	// Lock the matrix constant buffer so it can be written to.
	result = aDeviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	// Get a pointer to the data in the matrix constant buffer.
	matrixBufferType = (MatrixBufferType*)mappedResource.pData;
	// Copy the matrices into the matrix constant buffer.
	matrixBufferType->world = aWorldMatrix;
	matrixBufferType->view = aViewMatrix;
	matrixBufferType->projection = aProjectionMatrix;
	// Unlock the matrix constant buffer.
	aDeviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the matrix constant buffer in the vertex shader.
	bufferNumber = 0;
	// Now set the matrix constant buffer in the vertex shader with the updated values.
	aDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	// Lock the reflection constant buffer so it can be written to.
	result = aDeviceContext->Map(m_reflectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	// Get a pointer to the data in the matrix constant buffer.
	reflectionBufferType = (ReflectionBufferType*)mappedResource.pData;
	// Copy the matrix into the reflection constant buffer.
	reflectionBufferType->reflectionMatrix = aReflectionMatrix;
	// Unlock the reflection constant buffer.
	aDeviceContext->Unmap(m_reflectionBuffer, 0);

	// Set the position of the reflection constant buffer in the vertex shader.
	bufferNumber = 1;
	// Now set the reflection constant buffer in the vertex shader with the updated values.
	aDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_reflectionBuffer);
	// Set shader texture resource in the pixel shader.
	aDeviceContext->PSSetShaderResources(0, 1, &aTexture);
	// Set the reflection texture resource in the pixel shader.
	aDeviceContext->PSSetShaderResources(1, 1, &aReflectionTexture);

	return true;
}

void ReflectionShaderClass::RenderShader(ID3D11DeviceContext *aDeviceContext, int aIndexCount)
{
	// Set the vertex input layout.
	aDeviceContext->IASetInputLayout(m_layout);
	// Set the vertex and pixel shaders that will be used to render this triangle.
	aDeviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	aDeviceContext->PSSetShader(m_pixelShader, nullptr, 0);
	// Set the sampler state in the pixel shader.
	aDeviceContext->PSSetSamplers(0, 1, &m_sampleState);
	// Render the triangle.
	aDeviceContext->DrawIndexed(aIndexCount, 0, 0);
}