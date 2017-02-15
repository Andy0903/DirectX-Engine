#ifndef GRPAHICSCLASS_H
#define GRAPHICSCLASS_H

#include "Direct3DClass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "colorshaderclass.h"
#include "TextureShaderClass.h"
#include "LightShaderClass.h"
#include "LightClass.h"
#include "InputClass.h"
#include "PositionClass.h"
#include "BumpMapShaderClass.h"
#include "BitMapClass.h"
#include "DebugWindowClass.h"
#include "RenderTextureClass.h"
#include "reflectionshaderclass.h"
#include "DepthShaderClass.h"
#include "ShadowShaderClass.h"

const bool FULL_SCREEN = false; //true
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 100.0f; //1000
const float SCREEN_NEAR = 1.0f; //0.1

const int SHADOWMAP_WIDTH = 1024;
const int SHADOWMAP_HEIGHT = 1024;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND, InputClass*);
	void Shutdown();
	bool Frame(int, int, float);
	bool Render();

private:
	bool RenderScene();
	bool RenderSceneToTexture();

	Direct3DClass *myDirect3D;
	CameraClass *myCamera;
	ModelClass *myCube;
	ModelClass *mySphere;
	ModelClass *myGround;
	//ColorShaderClass *myColorShader;
	TextureShaderClass *myTextureShader;
	//LightShaderClass *myLightShader;
	BumpMapShaderClass *myBumpMapShader;
	LightClass *myLight;
	//BitmapClass *myBitmap;
	RenderTextureClass *myRenderTexture;
	//ReflectionShaderClass *myReflectionShader;
	//DebugWindowClass *myDebugWindow;
	DepthShaderClass *myDepthShader;
	ShadowShaderClass *myShadowShader;

	///FPSMovement
	enum Direction
	{
		Right,
		Left,
		Up,
		Down
	};
	bool DragsWithMouse(Direction aDirection);
	void HandleInput(float aDt);
	PositionClass *myPosition;
	InputClass *myInput;
	///
};

#endif;