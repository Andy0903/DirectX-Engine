#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
	myDirect3D = nullptr;
	myCamera = nullptr;
	myModel = nullptr;
	//myColorShader = nullptr;
	//myTextureShader = nullptr;
	myLightShader = nullptr;
	myLight = nullptr;
}

GraphicsClass::GraphicsClass(const GraphicsClass& aOther)
{
}

GraphicsClass::~GraphicsClass()
{
}

//Distance
bool GraphicsClass::Initialize(int aScreenWidth, int aScreenHeight, HWND aHwnd)
{
	bool result;

	myDirect3D = new Direct3DClass;
	result = myDirect3D->Initialize(aScreenWidth, aScreenHeight, VSYNC_ENABLED, aHwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(aHwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return result;
	}

	myCamera = new CameraClass;
	myCamera->SetPosition(0.0f, 0.0f, -5.0f);

	myModel = new ModelClass;
	char* modelPath = "../Engine/Models/Cube.txt";
	WCHAR* texturePath = L"../Engine/Textures/seafloor.dds";
	result = myModel->Initialize(myDirect3D->GetDevice(), modelPath, texturePath);
	if (!result)
	{
		MessageBox(aHwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return result;
	}

	/*myColorShader = new ColorShaderClass;
	result = myColorShader->Initialize(myDirect3D->GetDevice(), aHwnd);
	if (!result)
	{
		MessageBox(aHwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return result;
	}*/

	/*myTextureShader = new TextureShaderClass;
	if (myTextureShader == false)
	{
		return false;
	}
	result = myTextureShader->Initialize(myDirect3D->GetDevice(), aHwnd);
	if (!result)
	{
		MessageBox(aHwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}*/

	myLightShader = new LightShaderClass;
	if (!myLightShader) { return false; }

	result = myLightShader->Initialize(myDirect3D->GetDevice(), aHwnd);
	if (result == false)
	{
		MessageBox(aHwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	myLight = new LightClass;
	if (!myLight) { return false; }
	myLight->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	myLight->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	myLight->SetDirection(0.0f, 0.0f, 1.0f);

	return true;
}

void GraphicsClass::Shutdown()
{
	//if (myColorShader)
	//{
	//	myColorShader->Shutdown();
	//	delete myColorShader;
	//	myColorShader = nullptr;
	//}

	//if (myTextureShader)
	//{
	//	myTextureShader->Shutdown();
	//	delete myTextureShader;
	//	myTextureShader = nullptr;
	//}

	if (myLight)
	{
		delete myLight;
		myLight = nullptr;
	}

	if (myLightShader)
	{
		myLightShader->Shutdown();
		delete myLightShader;
		myLightShader = nullptr;
	}

	if (myModel)
	{
		myModel->Shutdown();
		delete myModel;
		myModel = nullptr;
	}

	if (myCamera)
	{
		delete myCamera;
		myCamera = nullptr;
	}

	if (myDirect3D)
	{
		myDirect3D->Shutdown();
		delete myDirect3D;
		myDirect3D = nullptr;
	}
}

bool GraphicsClass::Frame()
{
	bool result;
	static float rotation = 0.0f;
	rotation += (float)D3DX_PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	result = Render(rotation);
	return result;
}

bool GraphicsClass::Render(float aRotation)
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix;
	bool result;

	myDirect3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	myCamera->Render();

	myCamera->GetViewMatrix(viewMatrix);
	myDirect3D->GetWorldMatrix(worldMatrix);
	myDirect3D->GetProjectionMatrix(projectionMatrix);

	D3DXMatrixRotationY(&worldMatrix, aRotation);
	myModel->Render(myDirect3D->GetDeviceContext());

	//	result = myColorShader->Render(myDirect3D->GetDeviceContext(), myModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	//	result = myTextureShader->Render(myDirect3D->GetDeviceContext(), myModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, myModel->GetTexture());

	// Render the model using the light shader.
	result = myLightShader->Render(myDirect3D->GetDeviceContext(), myModel->GetIndexCount(), worldMatrix,
		viewMatrix, projectionMatrix, myModel->GetTexture(), myLight->GetDirection(), myLight->GetAmbientColor(), myLight->GetDiffuseColor());

	if (!result)
	{
		return false;
	}

	myDirect3D->EndScene();
	return true;
}