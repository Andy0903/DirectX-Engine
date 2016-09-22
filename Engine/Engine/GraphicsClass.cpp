#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
	myDirect3D = nullptr;
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
	}
	return result;
}

void GraphicsClass::Shutdown()
{
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
	myDirect3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	myDirect3D->EndScene();
	return true;
}