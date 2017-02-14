#include "DepthShaderClass.h"

DepthShaderClass::DepthShaderClass()
{
	myVertexShader = nullptr;
	myPixelShader = nullptr;
	myLayout = nullptr;
	myMatrixBuffer = nullptr;
}

DepthShaderClass::DepthShaderClass(const DepthShaderClass &aOther)
{
}

DepthShaderClass::~DepthShaderClass()
{
}
bool DepthShaderClass::Initialize(ID3D11Device *aDevice, HWND aHwnd)
{
	bool result;
	result = InitializeShader(aDevice, aHwnd, L"../Engine/depth.vs", L"../Engine/depth.ps");
	if (!result) { return false; }

	return true;
}

void DepthShaderClass::Shutdown()
{
	ShutdownShader();
}

bool DepthShaderClass::Render(ID3D11DeviceContext *aDeviceContext, int aIndexCount,
	D3DXMATRIX aWorldMatrix, D3DXMATRIX aViewMatrix, D3DXMATRIX aProjectionMatrix)
{
	bool result;
	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(aDeviceContext, aWorldMatrix, aViewMatrix, aProjectionMatrix);
	if (!result) { return false; }

	// Now render the prepared buffers with the shader.
	RenderShader(aDeviceContext, aIndexCount);

	return true;
}

bool DepthShaderClass::InitializeShader(ID3D11Device *device, HWND hwnd, WCHAR *vsFilename, WCHAR *psFilename)
{
	HRESULT result;
	ID3D10Blob *errorMessage;
	ID3D10Blob *vertexShaderBuffer;
	ID3D10Blob *pixelShaderBuffer;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	errorMessage = nullptr;
	vertexShaderBuffer = nullptr;
	pixelShaderBuffer = nullptr;

	// Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, nullptr, nullptr, "DepthVertexShader", "vs_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &vertexShaderBuffer, &errorMessage, nullptr);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the pixel shader code.
	result = D3DX11CompileFromFile(psFilename, nullptr, nullptr, "DepthPixelShader", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr, &pixelShaderBuffer, &errorMessage, nullptr);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &myVertexShader);
	if (FAILED(result)) { return false; }

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &myPixelShader);
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

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &myLayout);
	if (FAILED(result)) { return false; }

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, nullptr, &myMatrixBuffer);
	if (FAILED(result)) { return false; }

	return true;
}

void DepthShaderClass::ShutdownShader()
{
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

void DepthShaderClass::OutputShaderErrorMessage(ID3D10Blob *aErrorMessage, HWND aHwnd, WCHAR *aShaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	std::ofstream fout;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(aErrorMessage->GetBufferPointer());
	// Get the length of the message.
	bufferSize = aErrorMessage->GetBufferSize();
	// Open a file to write the error message to.
	fout.open("shader-error.txt");
	// Write out the error message.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	aErrorMessage->Release();
	aErrorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(aHwnd, L"Error compiling shader.  Check shader-error.txt for message.", aShaderFilename, MB_OK);
}

bool DepthShaderClass::SetShaderParameters(ID3D11DeviceContext *aDeviceContext, D3DXMATRIX aWorldMatrix, D3DXMATRIX aViewMatrix, D3DXMATRIX aProjectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* matrixBuffer;

	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&aWorldMatrix, &aWorldMatrix);
	D3DXMatrixTranspose(&aViewMatrix, &aViewMatrix);
	D3DXMatrixTranspose(&aProjectionMatrix, &aProjectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = aDeviceContext->Map(myMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	// Get a pointer to the data in the constant buffer.
	matrixBuffer = (MatrixBufferType*)mappedResource.pData;
	// Copy the matrices into the constant buffer.
	matrixBuffer->world = aWorldMatrix;
	matrixBuffer->view = aViewMatrix;
	matrixBuffer->projection = aProjectionMatrix;
	// Unlock the constant buffer.
	aDeviceContext->Unmap(myMatrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	aDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &myMatrixBuffer);

	return true;
}

void DepthShaderClass::RenderShader(ID3D11DeviceContext *aDeviceContext, int aIndexCount)
{
	// Set the vertex input layout.
	aDeviceContext->IASetInputLayout(myLayout);
	// Set the vertex and pixel shaders that will be used to render this triangle.
	aDeviceContext->VSSetShader(myVertexShader, nullptr, 0);
	aDeviceContext->PSSetShader(myPixelShader, nullptr, 0);
	// Render the triangle.
	aDeviceContext->DrawIndexed(aIndexCount, 0, 0);
}