//#include "ColorShaderClass.h"
//ColorShaderClass::ColorShaderClass()
//{
//	myVertexShader = nullptr;
//	myPixelShader = nullptr;
//	myLayout = nullptr;
//	myMatrixBuffer = nullptr;
//}
//
//ColorShaderClass::ColorShaderClass(const ColorShaderClass& aOther)
//{
//}
//
//ColorShaderClass::~ColorShaderClass()
//{
//}
//
//bool ColorShaderClass::Initialize(ID3D11Device* aDevice, HWND aHwnd)
//{
//	return InitializeShader(aDevice, aHwnd, L"../Engine/Color.vs", L"../Engine/Color.ps");
//}
//
//void ColorShaderClass::Shutdown()
//{
//	ShutdownShader();
//}
//
//bool ColorShaderClass::Render(ID3D11DeviceContext* aDeviceContext, int aIndexCount, D3DXMATRIX aWorldMatrix, D3DXMATRIX aViewMatrix, D3DXMATRIX aProjectionMatrix)
//{
//	SetShaderParameters(aDeviceContext, aWorldMatrix, aViewMatrix, aProjectionMatrix);
//	RenderShader(aDeviceContext, aIndexCount);
//
//	return true;
//}
//
//bool ColorShaderClass::InitializeShader(ID3D11Device* aDevice, HWND aHwnd, WCHAR* aVsFileName, WCHAR* aPsFileName)
//{
//	HRESULT result;
//	ID3D10Blob* errorMessage = nullptr;
//	ID3D10Blob* vertexShaderBuffer = nullptr;
//	ID3D10Blob* pixelShaderBuffer = nullptr;
//	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
//	unsigned int numElements;
//	D3D11_BUFFER_DESC matrixBufferDesc;
//
//	result = D3DX11CompileFromFile(aVsFileName, nullptr, nullptr, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr,
//		&vertexShaderBuffer, &errorMessage, nullptr);
//	if (FAILED(result))
//	{
//		if (errorMessage)
//		{
//			OutputShaderErrorMessage(errorMessage, aHwnd, aVsFileName);
//		}
//		else
//		{
//			MessageBox(aHwnd, aVsFileName, L"Missing Shader File", MB_OK);
//		}
//
//		return false;
//	}
//
//	result = D3DX11CompileFromFile(aPsFileName, nullptr, nullptr, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr,
//		&pixelShaderBuffer, &errorMessage, nullptr);
//	if (FAILED(result))
//	{
//		if (errorMessage)
//		{
//			OutputShaderErrorMessage(errorMessage, aHwnd, aPsFileName);
//		}
//		else
//		{
//			MessageBox(aHwnd, aPsFileName, L"Missing Shader File", MB_OK);
//		}
//		
//		return false;
//	}
//
//	result = aDevice->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &myVertexShader);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	result = aDevice->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &myPixelShader);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	polygonLayout[0].SemanticName = "POSITION";
//	polygonLayout[0].SemanticIndex = 0;
//	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
//	polygonLayout[0].InputSlot = 0;
//	polygonLayout[0].AlignedByteOffset = 0;
//	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
//	polygonLayout[0].InstanceDataStepRate = 0;
//
//	polygonLayout[1].SemanticName = "COLOR";
//	polygonLayout[1].SemanticIndex = 0;
//	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//	polygonLayout[1].InputSlot = 0;
//	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
//	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
//	polygonLayout[1].InstanceDataStepRate = 0;
//
//	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);
//	result = aDevice->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &myLayout);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	vertexShaderBuffer->Release();
//	vertexShaderBuffer = nullptr;
//
//	pixelShaderBuffer->Release();
//	pixelShaderBuffer = nullptr;
//
//	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
//	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//	matrixBufferDesc.MiscFlags = 0;
//	matrixBufferDesc.StructureByteStride = 0;
//
//	result = aDevice->CreateBuffer(&matrixBufferDesc, NULL, &myMatrixBuffer);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	return true;
//}
//
//void ColorShaderClass::ShutdownShader()
//{
//	if (myMatrixBuffer)
//	{
//		myMatrixBuffer->Release();
//		myMatrixBuffer = nullptr;
//	}
//
//	if (myLayout)
//	{
//		myLayout->Release();
//		myLayout = nullptr;
//	}
//
//	if (myPixelShader)
//	{
//		myPixelShader->Release();
//		myPixelShader = nullptr;
//	}
//
//	if (myVertexShader)
//	{
//		myVertexShader->Release();
//		myVertexShader = nullptr;
//	}
//}
//
//void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND aHwnd, WCHAR* aShaderFileName)
//{
//	char* compileErrors;
//	unsigned long bufferSize;
//	std::ofstream fileOut;
//
//	compileErrors = (char*)(errorMessage->GetBufferPointer());
//	bufferSize = errorMessage->GetBufferSize();
//
//	fileOut.open("Shader-error.txt");
//	for (int i = 0; i < bufferSize; ++i)
//	{
//		fileOut << compileErrors[i];
//	}
//	fileOut.close();
//
//	errorMessage->Release();
//	errorMessage = nullptr;
//
//	MessageBox(aHwnd, L"Error compiling shader. Check Shader-error.txt for message.", aShaderFileName, MB_OK);
//}
//
//bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* aDeviceContext, D3DXMATRIX aWorldMatrix, D3DXMATRIX aViewMatrix, D3DXMATRIX aProjectionMatrix)
//{
//	HRESULT result;
//	D3D11_MAPPED_SUBRESOURCE mappedResource;
//	MatrixBufferType* dataPtr;
//	unsigned int bufferNumber = 0;
//
//	D3DXMatrixTranspose(&aWorldMatrix, &aWorldMatrix);
//	D3DXMatrixTranspose(&aViewMatrix, &aViewMatrix);
//	D3DXMatrixTranspose(&aProjectionMatrix, &aProjectionMatrix);
//
//	result = aDeviceContext->Map(myMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	dataPtr = (MatrixBufferType*)mappedResource.pData;
//
//	dataPtr->world = aWorldMatrix;
//	dataPtr->view = aViewMatrix;
//	dataPtr->projection = aProjectionMatrix;
//
//	aDeviceContext->Unmap(myMatrixBuffer, 0);
//
//	aDeviceContext->VSSetConstantBuffers(bufferNumber, 1, &myMatrixBuffer);
//}
//
//void ColorShaderClass::RenderShader(ID3D11DeviceContext* aDeviceContext, int aIndexCount)
//{
//	aDeviceContext->IASetInputLayout(myLayout);
//	aDeviceContext->VSSetShader(myVertexShader, nullptr, 0);
//	aDeviceContext->PSSetShader(myPixelShader, nullptr, 0);
//	aDeviceContext->DrawIndexed(aIndexCount, 0, 0);
//}