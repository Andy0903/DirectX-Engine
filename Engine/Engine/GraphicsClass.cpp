#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
	myDirect3D = nullptr;
	myCamera = nullptr;
	myModel = nullptr;
	myColorShader = nullptr;
}

GraphicsClass::GraphicsClass(const GraphicsClass& aOther)
{
}

GraphicsClass::~GraphicsClass()
{
}

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
	myCamera->SetPosition(0.0f, 0.0f, -20.0f);

	myModel = new ModelClass;
	result = myModel->Initialize(myDirect3D->GetDevice());
	if (!result)
	{
		MessageBox(aHwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return result;
	}

	myColorShader = new ColorShaderClass;
	result = myColorShader->Initialize(myDirect3D->GetDevice(), aHwnd);
	if (!result)
	{
		MessageBox(aHwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return result;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	if (myColorShader)
	{
		myColorShader->Shutdown();
		delete myColorShader;
		myColorShader = nullptr;
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
	return Render();
}

bool GraphicsClass::Render()
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix;
	bool result;

	myDirect3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	myCamera->Render();

	myCamera->GetViewMatrix(viewMatrix);
	myDirect3D->GetWorldMatrix(worldMatrix);
	myDirect3D->GetProjectionMatrix(projectionMatrix);

	myModel->Render(myDirect3D->GetDeviceContext());

	result = myColorShader->Render(myDirect3D->GetDeviceContext(), myModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);

	myDirect3D->EndScene();
	return result;
}