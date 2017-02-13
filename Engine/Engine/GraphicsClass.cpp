#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
	myDirect3D = nullptr;
	myCamera = nullptr;
	myModel = nullptr;
	//myColorShader = nullptr;
	myTextureShader = nullptr;
	//myLightShader = nullptr;
	myBumpMapShader = nullptr;
	myLight = nullptr;
	myPosition = nullptr;
	myBitmap = nullptr;
}

GraphicsClass::GraphicsClass(const GraphicsClass& aOther)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int aScreenWidth, int aScreenHeight, HWND aHwnd, InputClass *aInput)
{
	bool result;
	myInput = aInput;

	myDirect3D = new Direct3DClass;
	result = myDirect3D->Initialize(aScreenWidth, aScreenHeight, VSYNC_ENABLED, aHwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(aHwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return result;
	}

	myCamera = new CameraClass;
	myCamera->SetPosition(0.0f, 0.0f, -5.0f);

	myPosition = new PositionClass();
	myPosition->SetPosition(myCamera->GetPosition().x, myCamera->GetPosition().y, myCamera->GetPosition().z);

	myModel = new ModelClass;
	char* modelPath = "../Engine/Models/Cube.txt";
	WCHAR* textureColorPath = L"../Engine/Textures/Brick.dds";
	WCHAR* textureBumpPath = L"../Engine/Textures/BrickBump.dds";
	result = myModel->Initialize(myDirect3D->GetDevice(), modelPath, textureColorPath, textureBumpPath);
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

	myTextureShader = new TextureShaderClass;
	if (myTextureShader == false)
	{
		return false;
	}
	result = myTextureShader->Initialize(myDirect3D->GetDevice(), aHwnd);
	if (!result)
	{
		MessageBox(aHwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	//myLightShader = new LightShaderClass;
	//if (!myLightShader) { return false; }
	//result = myLightShader->Initialize(myDirect3D->GetDevice(), aHwnd);
	//if (result == false)
	//{
	//	MessageBox(aHwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
	//	return false;
	//}

	myBumpMapShader = new BumpMapShaderClass;
	result = myBumpMapShader->Initialize(myDirect3D->GetDevice(), aHwnd);
	if (result == false)
	{
		MessageBox(aHwnd, L"Could not initialize the bump map shader object.", L"Error", MB_OK);
		return false;
	}

	myLight = new LightClass;
	if (!myLight) { return false; }
	myLight->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	myLight->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	myLight->SetDirection(0.0f, 0.0f, 1.0f);
	myLight->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	myLight->SetSpecularPower(32.0f);

	myBitmap = new BitmapClass;
	result = myBitmap->Initialize(myDirect3D->GetDevice(), aScreenWidth, aScreenHeight, L"../Engine/Textures/Red.dds", 256, 256);
	if (!result)
	{
		MessageBox(aHwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

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

	if (myTextureShader)
	{
		myTextureShader->Shutdown();
		delete myTextureShader;
		myTextureShader = nullptr;
	}

	//if (myLightShader)
	//{
	//	myLightShader->Shutdown();
	//	delete myLightShader;
	//	myLightShader = nullptr;
	//}

	if (myBitmap)
	{
		myBitmap->Shutdown();
		delete myBitmap;
		myBitmap = nullptr;
	}

	if (myBumpMapShader)
	{
		myBumpMapShader->Shutdown();
		delete myBumpMapShader;
		myBumpMapShader = nullptr;
	}

	if (myPosition)
	{
		delete myPosition;
		myPosition = nullptr;
	}

	if (myLight)
	{
		delete myLight;
		myLight = nullptr;
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

bool GraphicsClass::DragsWithMouse(Direction aDirection)
{
	switch (aDirection)
	{
	case Direction::Left:
		return (myInput->GetMouseDeltaX() < -5);
	case Direction::Right:
		return (myInput->GetMouseDeltaX() > 5);
	case Direction::Up:
		return (myInput->GetMouseDeltaY() < -5);
	case Direction::Down:
		return (myInput->GetMouseDeltaY() > 5);
	}
}

bool GraphicsClass::Frame(int aMouseX, int aMouseY, float aDt)
{
	bool result;
	static float rotation = 0.0f;
	rotation += (float)D3DX_PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}
	result = Render(rotation);

	HandleInput(aDt);

	return result;
}

void GraphicsClass::HandleInput(float aDt)
{
	bool keyDown, result;
	float posX, posY, posZ, rotX, rotY, rotZ;

	if (DragsWithMouse(Direction::Up)) { myPosition->LookUpward(aDt); }
	if (DragsWithMouse(Direction::Down)) { myPosition->LookDownward(aDt); }
	if (DragsWithMouse(Direction::Left)) { myPosition->TurnLeft(aDt); }
	if (DragsWithMouse(Direction::Right)) { myPosition->TurnRight(aDt); }

	if (myInput->IsKeyPressed(DIK_D)) { myPosition->MoveRight(aDt); }
	if (myInput->IsKeyPressed(DIK_A)) { myPosition->MoveLeft(aDt); }
	if (myInput->IsKeyPressed(DIK_W)) { myPosition->MoveForward(aDt); }
	if (myInput->IsKeyPressed(DIK_S)) { myPosition->MoveBackward(aDt); }
	if (myInput->IsKeyPressed(DIK_Q)) { myPosition->MoveUpward(aDt); }
	if (myInput->IsKeyPressed(DIK_E)) { myPosition->MoveDownward(aDt); }

	// Get the view point position/rotation.
	myPosition->GetPosition(posX, posY, posZ);
	myPosition->GetRotation(rotX, rotY, rotZ);

	// Set the position of the camera.
	myCamera->SetPosition(posX, posY, posZ);
	myCamera->SetRotation(rotX, rotY, rotZ);
}

bool GraphicsClass::Render(float aRotation)
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix, orthoMatrix;
	bool result;

	myDirect3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	myCamera->Render();

	myCamera->GetViewMatrix(viewMatrix);
	myDirect3D->GetWorldMatrix(worldMatrix);
	myDirect3D->GetProjectionMatrix(projectionMatrix);

	myDirect3D->GetOrthoMatrix(orthoMatrix);

	D3DXMatrixRotationY(&worldMatrix, aRotation);
	myModel->Render(myDirect3D->GetDeviceContext());

	//	result = myColorShader->Render(myDirect3D->GetDeviceContext(), myModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	//	result = myTextureShader->Render(myDirect3D->GetDeviceContext(), myModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, myModel->GetTexture());

	// Render the model using the light shader.
	//result = myLightShader->Render(myDirect3D->GetDeviceContext(), myModel->GetIndexCount(), worldMatrix,
	//	viewMatrix, projectionMatrix, myModel->GetTexture(), myLight->GetDirection(), myLight->GetAmbientColor(), myLight->GetDiffuseColor(),
	//	myCamera->GetPosition(), myLight->GetSpecularColor(), myLight->GetSpecularPower());

	//Render the model using the bump map shader.
	result = myBumpMapShader->Render(myDirect3D->GetDeviceContext(), myModel->GetIndexCount(), worldMatrix,
		viewMatrix, projectionMatrix, myModel->GetTextureArray(), myLight->GetDirection(), myLight->GetDiffuseColor());

	if (!result) { return false; }

	myDirect3D->TurnZBufferOff();

	result = myBitmap->Render(myDirect3D->GetDeviceContext(), 100, 100);
	if (result == false) { return false; }
	result = myTextureShader->Render(myDirect3D->GetDeviceContext(), myBitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, myBitmap->GetTexture());
	if (result == false) { return false; }

	myDirect3D->TurnZBufferOn();

	myDirect3D->EndScene();
	return true;
}