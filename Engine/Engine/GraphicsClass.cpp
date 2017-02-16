#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
	myDirect3D = nullptr;
	myCamera = nullptr;
	myPosition = nullptr;

	myCube = nullptr;
	myGround = nullptr;
	mySphere = nullptr;
	myLight = nullptr;

	myRenderTexture = nullptr;
	myDepthShader = nullptr;
	myShadowShader = nullptr;

	//myBitmap = nullptr;
	//myReflectionShader = nullptr;
	//myColorShader = nullptr;
	//myTextureShader = nullptr;
	//myLightShader = nullptr;
	//myBumpMapShader = nullptr;
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
	myCamera->SetPosition(0.0f, 2.0f, -10.0f);
	myPosition = new PositionClass();
	myPosition->SetPosition(myCamera->GetPosition().x, myCamera->GetPosition().y, myCamera->GetPosition().z);

	myCube = new ModelClass;
	char* modelPath = "../Engine/Models/Cube.txt";
	WCHAR* textureColorPath = L"../Engine/Textures/Blue.dds";
	WCHAR* textureBumpPath = L"../Engine/Textures/BrickBump.dds";
	result = myCube->Initialize(myDirect3D->GetDevice(), modelPath, textureColorPath, textureBumpPath);
	if (!result)
	{
		MessageBox(aHwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return result;
	}
	myCube->SetPosition(-2, 2, 0);

	mySphere = new ModelClass;
	modelPath = "../Engine/Models/Sphere.txt";
	textureColorPath = L"../Engine/Textures/Seafloor.dds";
	result = mySphere->Initialize(myDirect3D->GetDevice(), modelPath, textureColorPath, textureBumpPath);
	if (!result)
	{
		MessageBox(aHwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return result;
	}
	mySphere->SetPosition(2, 2, 0);

	myLight = new LightClass;
	if (!myLight) { return false; }
	myLight->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	myLight->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	//myLight->SetDirection(0.0f, 0.0f, 1.0f);
	//myLight->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	//myLight->SetSpecularPower(32.0f);
	myLight->SetLookAt(0, 0, 0);
	myLight->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);

	myRenderTexture = new RenderTextureClass;
	result = myRenderTexture->Initialize(myDirect3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) { return false; }

	myGround = new ModelClass;
	if (!myGround) { return false; }
	result = myGround->Initialize(myDirect3D->GetDevice(), "../Engine/Models/Floor.txt", L"../Engine/Textures/Red.dds", L"../Engine/Textures/Red.dds");
	if (!result)
	{
		MessageBox(aHwnd, L"Could not initialize the floor model object.", L"Error", MB_OK);
		return false;
	}
	myGround->SetPosition(0, 1, 0);

	// Create the depth shader object.
	myDepthShader = new DepthShaderClass;
	if (!myDepthShader)
	{
		return false;
	}

	// Initialize the depth shader object.
	result = myDepthShader->Initialize(myDirect3D->GetDevice(), aHwnd);
	if (!result)
	{
		MessageBox(aHwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK);
		return false;
	}

	myShadowShader = new ShadowShaderClass;
	result = myShadowShader->Initialize(myDirect3D->GetDevice(), aHwnd);
	if (!result)
	{
		MessageBox(aHwnd, L"Could not initialize the shadow shader object.", L"Error", MB_OK);
		return false;
	}

	return true;

	/*myColorShader = new ColorShaderClass;
	result = myColorShader->Initialize(myDirect3D->GetDevice(), aHwnd);
	if (!result)
	{
	MessageBox(aHwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
	return result;
	}
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
	myLightShader = new LightShaderClass;
	if (!myLightShader) { return false; }
	result = myLightShader->Initialize(myDirect3D->GetDevice(), aHwnd);
	if (result == false)
	{
		MessageBox(aHwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}
	myBumpMapShader = new BumpMapShaderClass;
	result = myBumpMapShader->Initialize(myDirect3D->GetDevice(), aHwnd);
	if (result == false)
	{
		MessageBox(aHwnd, L"Could not initialize the bump map shader object.", L"Error", MB_OK);
		return false;
	}
	myBitmap = new BitmapClass;
	result = myBitmap->Initialize(myDirect3D->GetDevice(), aScreenWidth, aScreenHeight, L"../Engine/Textures/Red.dds", 256, 256);
	if (!result)
	{
		MessageBox(aHwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}
	myDebugWindow = new DebugWindowClass;
	result = myDebugWindow->Initialize(myDirect3D->GetDevice(), aScreenWidth, aScreenHeight, 160, 120);
	if (!result)
	{
		MessageBox(aHwnd, L"Could not initialize the debug window object.", L"Error", MB_OK);
		return false;
	}
	// Create the reflection shader object.
	myReflectionShader = new ReflectionShaderClass;
	if (!myReflectionShader) { return false; }
	// Initialize the reflection shader object.
	result = myReflectionShader->Initialize(myDirect3D->GetDevice(), aHwnd);
	if (!result)
	{
		MessageBox(aHwnd, L"Could not initialize the reflection shader object.", L"Error", MB_OK);
		return false;
	}*/
}

void GraphicsClass::Shutdown()
{
	if (myDepthShader)
	{
		myDepthShader->Shutdown();
		delete myDepthShader;
		myDepthShader = nullptr;
	}

	if (myShadowShader)
	{
		myShadowShader->Shutdown();
		delete myShadowShader;
		myShadowShader = nullptr;
	}

	if (myGround)
	{
		myGround->Shutdown();
		delete myGround;
		myGround = nullptr;
	}

	if (myRenderTexture)
	{
		myRenderTexture->Shutdown();
		delete myRenderTexture;
		myRenderTexture = nullptr;
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

	if (myCube)
	{
		myCube->Shutdown();
		delete myCube;
		myCube = nullptr;
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

	//if (myLightShader)
	//{
	//	myLightShader->Shutdown();
	//	delete myLightShader;
	//	myLightShader = nullptr;
	//}
	//if (myBitmap)
	//{
	//	myBitmap->Shutdown();
	//	delete myBitmap;
	//	myBitmap = nullptr;
	//}
	//if (myBumpMapShader)
	//{
	//	myBumpMapShader->Shutdown();
	//	delete myBumpMapShader;
	//	myBumpMapShader = nullptr;
	//}
	//if (myReflectionShader)
	//{
	//	myReflectionShader->Shutdown();
	//	delete myReflectionShader;
	//	myReflectionShader = nullptr;
	//}
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
	//if (myDebugWindow)
	//{
	//	myDebugWindow->Shutdown();
	//	delete myDebugWindow;
	//	myDebugWindow = nullptr;
	//}
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

int LightSpeed = 1;
bool GraphicsClass::Frame(int aMouseX, int aMouseY, float aDt)
{
	bool result;
	static float lightPositionX = -5.0f;
	HandleInput(aDt);

	lightPositionX += LightSpeed * aDt;
	if (lightPositionX > 5.0f || lightPositionX < -5.0f)
	{
		LightSpeed *= -1;
	}
	myLight->SetPosition(lightPositionX, 8.0f, -5.0f);

	result = Render();
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

//bool GraphicsClass::Render()
//{
//	bool result;
//	// Render the entire scene as a reflection to the texture first.
//	//result = RenderToTexture();
//	//if (!result) { return false; }
//	// Render the scene as normal to the back buffer.
//	//result = RenderScene();
//	//if (!result) { return false; }
//
//	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
//
//	// Clear the buffers to begin the scene.
//	myDirect3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
//
//	// Generate the view matrix based on the camera's position.
//	myCamera->Render();
//
//	// Get the world, view, and projection matrices from the camera and d3d objects.
//	myCamera->GetViewMatrix(viewMatrix);
//	myDirect3D->GetWorldMatrix(worldMatrix);
//	myDirect3D->GetProjectionMatrix(projectionMatrix);
//
//	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
//	myGround->Render(myDirect3D->GetDeviceContext());
//
//	// Render the model using the depth shader.
//	result = myDepthShader->Render(myDirect3D->GetDeviceContext(), myGround->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
//	if (!result) { return false; }
//
//	// Present the rendered scene to the screen.
//	myDirect3D->EndScene();
//
//	return true;
//
//
//	//D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix, orthoMatrix, unrotatedWorldMatrix;
//	//bool result;
//	//result = RenderToTexture();
//	//if (!result) { return false; }
//	//myDirect3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
//	//result = RenderScene();
//	//if (!result) { return false; }
//	//myDirect3D->TurnZBufferOff();
//	//myDirect3D->GetWorldMatrix(worldMatrix);
//	//myCamera->GetViewMatrix(viewMatrix);
//	//myDirect3D->GetOrthoMatrix(orthoMatrix);
//	//D3DXMATRIX rot;
//	//myCamera->GetRotationMatrix(rot);
//	//myDirect3D->TurnZBufferOn();
//	//myDirect3D->EndScene();
//
//	//result = myDebugWindow->Render(myDirect3D->GetDeviceContext(), 50, 50);
//	//if (!result) { return false; }
//	//result = myTextureShader->Render(myDirect3D->GetDeviceContext(), myDebugWindow->GetIndexCount(), worldMatrix * rot, viewMatrix,
//	//	orthoMatrix, myRenderTexture->GetShaderResourceView());
//	//if (!result) { return false; }
//
//	//	result = myColorShader->Render(myDirect3D->GetDeviceContext(), myModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
//	//	result = myTextureShader->Render(myDirect3D->GetDeviceContext(), myModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, myModel->GetTexture());
//
//	// Render the model using the light shader.
//	//result = myLightShader->Render(myDirect3D->GetDeviceContext(), myModel->GetIndexCount(), worldMatrix,
//	//	viewMatrix, projectionMatrix, myModel->GetTexture(), myLight->GetDirection(), myLight->GetAmbientColor(), myLight->GetDiffuseColor(),
//	//	myCamera->GetPosition(), myLight->GetSpecularColor(), myLight->GetSpecularPower());
//
//	//result = myBitmap->Render(myDirect3D->GetDeviceContext(), 100, 100);
//	//if (result == false) { return false; }
//
//	//D3DXMATRIX rotMatrix;
//	//myCamera->GetRotationMatrix(rotMatrix);
//	//result = myTextureShader->Render(myDirect3D->GetDeviceContext(), myBitmap->GetIndexCount(), unrotatedWorldMatrix * rotMatrix, viewMatrix, orthoMatrix, myBitmap->GetTexture());
//	//if (result == false) { return false; }
//}
//
////bool GraphicsClass::RenderToTexture()
////{
////	//bool result;
////	//// Set the render target to be the render to texture.
////	//myRenderTexture->SetRenderTarget(myDirect3D->GetDeviceContext(), myDirect3D->GetDepthStencilView());
////	//// Clear the render to texture.
////	//myRenderTexture->ClearRenderTarget(myDirect3D->GetDeviceContext(), myDirect3D->GetDepthStencilView(), 0.0f, 0.0f, 1.0f, 1.0f);
////	//// Render the scene now and it will draw to the render to texture instead of the back buffer.
////	//result = RenderScene();
////	//if (!result) { return false; }
////	//// Reset the render target back to the original back buffer and not the render to texture anymore.
////	//myDirect3D->SetBackBufferRenderTarget();
////
////	D3DXMATRIX worldMatrix, reflectionViewMatrix, projectionMatrix;
////	static float rotation = 0.0f;
////	// Set the render target to be the render to texture.
////	myRenderTexture->SetRenderTarget(myDirect3D->GetDeviceContext(), myDirect3D->GetDepthStencilView());
////	// Clear the render to texture.
////	myRenderTexture->ClearRenderTarget(myDirect3D->GetDeviceContext(), myDirect3D->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);
////	// Use the camera to calculate the reflection matrix.
////	myCamera->RenderReflection(-1.5f);
////
////
////	// Get the camera reflection view matrix instead of the normal view matrix.
////	reflectionViewMatrix = myCamera->GetReflectionViewMatrix();
////	// Get the world and projection matrices.
////	myDirect3D->GetWorldMatrix(worldMatrix);
////	myDirect3D->GetProjectionMatrix(projectionMatrix);
////	// Update the rotation variable each frame.
////	rotation += (float)D3DX_PI * 0.005f;
////	if (rotation > 360.0f)
////	{
////		rotation -= 360.0f;
////	}
////	D3DXMatrixRotationY(&worldMatrix, rotation);
////	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
////	myCube->Render(myDirect3D->GetDeviceContext());
////	// Render the model using the texture shader and the reflection view matrix.
////	myTextureShader->Render(myDirect3D->GetDeviceContext(), myCube->GetIndexCount(), worldMatrix, reflectionViewMatrix,
////		projectionMatrix, myCube->GetTextureArray()[0]);
////	// Reset the render target back to the original back buffer and not the render to texture anymore.
////	myDirect3D->SetBackBufferRenderTarget();
////
////	return true;
////}
////
////bool GraphicsClass::RenderScene()
////{
////	//D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
////	//bool result;
////	//static float rotation = 0.0f;
////	//// Generate the view matrix based on the camera's position.
////	//myCamera->Render();
////	//// Get the world, view, and projection matrices from the camera and d3d objects.
////	//myDirect3D->GetWorldMatrix(worldMatrix);
////	//myCamera->GetViewMatrix(viewMatrix);
////	//myDirect3D->GetProjectionMatrix(projectionMatrix);
////	//// Update the rotation variable each frame.
////	//rotation += (float)D3DX_PI * 0.002f;
////	//if (rotation > 360.0f)
////	//{
////	//	rotation -= 360.0f;
////	//}
////	//D3DXMatrixRotationY(&worldMatrix, rotation);
////	//// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
////	//myModel->Render(myDirect3D->GetDeviceContext());
////	////Render the model using the bump map shader.
////	//result = myBumpMapShader->Render(myDirect3D->GetDeviceContext(), myModel->GetIndexCount(), worldMatrix,
////	//	viewMatrix, projectionMatrix, myModel->GetTextureArray(), myLight->GetDirection(), myLight->GetDiffuseColor());
////	//if (!result) { return false; }
////
////	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix;
////	bool result;
////	static float rotation = 0.0f;
////
////	// Clear the buffers to begin the scene.
////	myDirect3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
////	// Generate the view matrix based on the camera's position.
////	myCamera->Render();
////	// Get the world, view, and projection matrices from the camera and myDirect3D objects.
////	myDirect3D->GetWorldMatrix(worldMatrix);
////	myCamera->GetViewMatrix(viewMatrix);
////	myDirect3D->GetProjectionMatrix(projectionMatrix);
////
////	// Update the rotation variable each frame.
////	rotation += (float)D3DX_PI * 0.005f;
////	if (rotation > 360.0f)
////	{
////		rotation -= 360.0f;
////	}
////
////	// Multiply the world matrix by the rotation.
////	D3DXMatrixRotationY(&worldMatrix, rotation);
////	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
////	myCube->Render(myDirect3D->GetDeviceContext());
////	// Render the model with the texture shader.
////	//result = myTextureShader->Render(myDirect3D->GetDeviceContext(), myModel->GetIndexCount(), worldMatrix, viewMatrix,
////	//	projectionMatrix, myModel->GetTextureArray()[0]);
////	result = myBumpMapShader->Render(myDirect3D->GetDeviceContext(), myCube->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
////		myCube->GetTextureArray(), myLight->GetDirection(), myLight->GetDiffuseColor());
////	if (!result) { return false; }
////
////	// Get the world matrix again and translate down for the floor model to render underneath the cube.
////	myDirect3D->GetWorldMatrix(worldMatrix);
////	D3DXMatrixTranslation(&worldMatrix, 0.0f, -1.5f, 0.0f);
////	// Get the camera reflection view matrix.
////	reflectionMatrix = myCamera->GetReflectionViewMatrix();
////	// Put the floor model vertex and index buffers on the graphics pipeline to prepare them for drawing.
////	myGround->Render(myDirect3D->GetDeviceContext());
////	// Render the floor model using the reflection shader, reflection texture, and reflection view matrix.
////	result = myReflectionShader->Render(myDirect3D->GetDeviceContext(), myGround->GetIndexCount(), worldMatrix, viewMatrix,
////		projectionMatrix, myGround->GetTextureArray()[0], myRenderTexture->GetShaderResourceView(),
////		reflectionMatrix);
////
////	// Present the rendered scene to the screen.
////	myDirect3D->EndScene();
////
////	return true;
////}

bool GraphicsClass::RenderSceneToTexture()
{
	D3DXMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix, translateMatrix;
	float posX, posY, posZ;
	bool result;

	myRenderTexture->SetRenderTarget(myDirect3D->GetDeviceContext());
	myRenderTexture->ClearRenderTarget(myDirect3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	myLight->GenerateViewMatrix();
	myDirect3D->GetWorldMatrix(worldMatrix);
	myLight->GetViewMatrix(lightViewMatrix);
	myLight->GetProjectionMatrix(lightProjectionMatrix);

	myCube->GetPosition(posX, posY, posZ);
	D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);
	myCube->Render(myDirect3D->GetDeviceContext());
	result = myDepthShader->Render(myDirect3D->GetDeviceContext(), myCube->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	if (!result) { return false; }

	myDirect3D->GetWorldMatrix(worldMatrix);
	mySphere->GetPosition(posX, posY, posZ);
	D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);
	mySphere->Render(myDirect3D->GetDeviceContext());
	result = myDepthShader->Render(myDirect3D->GetDeviceContext(), mySphere->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	if (!result) { return false; }

	myDirect3D->GetWorldMatrix(worldMatrix);
	myGround->GetPosition(posX, posY, posZ);
	D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);
	myGround->Render(myDirect3D->GetDeviceContext());
	result = myDepthShader->Render(myDirect3D->GetDeviceContext(), myGround->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	if (!result) { return false; }

	myDirect3D->SetBackBufferRenderTarget();
	myDirect3D->ResetViewport();
	return true;
}

bool GraphicsClass::Render()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix;
	D3DXMATRIX lightViewMatrix, lightProjectionMatrix;
	bool result;
	float posX, posY, posZ;

	result = RenderSceneToTexture();
	if (!result) { return false; }

	myDirect3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	myCamera->Render();
	myLight->GenerateViewMatrix();
	myCamera->GetViewMatrix(viewMatrix);
	myDirect3D->GetWorldMatrix(worldMatrix);
	myDirect3D->GetProjectionMatrix(projectionMatrix);
	myLight->GetViewMatrix(lightViewMatrix);
	myLight->GetProjectionMatrix(lightProjectionMatrix);

	myCube->GetPosition(posX, posY, posZ);
	D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);
	myCube->Render(myDirect3D->GetDeviceContext());
	result = myShadowShader->Render(myDirect3D->GetDeviceContext(), myCube->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, myCube->GetTextureArray()[0], myRenderTexture->GetShaderResourceView(), myLight->GetPosition(),
		myLight->GetAmbientColor(), myLight->GetDiffuseColor());
	if (!result) { return false; }

	myDirect3D->GetWorldMatrix(worldMatrix);
	mySphere->GetPosition(posX, posY, posZ);
	D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);
	mySphere->Render(myDirect3D->GetDeviceContext());
	result = myShadowShader->Render(myDirect3D->GetDeviceContext(), mySphere->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, mySphere->GetTextureArray()[0], myRenderTexture->GetShaderResourceView(), myLight->GetPosition(),
		myLight->GetAmbientColor(), myLight->GetDiffuseColor());
	if (!result) { return false; }

	myDirect3D->GetWorldMatrix(worldMatrix);
	myGround->GetPosition(posX, posY, posZ);
	D3DXMatrixTranslation(&worldMatrix, posX, posY, posZ);
	myGround->Render(myDirect3D->GetDeviceContext());
	result = myShadowShader->Render(myDirect3D->GetDeviceContext(), myGround->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, myGround->GetTextureArray()[0], myRenderTexture->GetShaderResourceView(), myLight->GetPosition(),
		myLight->GetAmbientColor(), myLight->GetDiffuseColor());
	if (!result) { return false; }

	myDirect3D->EndScene();
	return true;
}