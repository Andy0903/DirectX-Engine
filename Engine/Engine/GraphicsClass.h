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

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

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
	bool RenderToTexture();

	Direct3DClass *myDirect3D;
	CameraClass *myCamera;
	ModelClass *myModel;
	//ColorShaderClass *myColorShader;
	TextureShaderClass *myTextureShader;
	//LightShaderClass *myLightShader;
	BumpMapShaderClass *myBumpMapShader;
	LightClass *myLight;
	//BitmapClass *myBitmap;
	RenderTextureClass *myRenderTexture;
	ModelClass *myFloorModel;
	ReflectionShaderClass *myReflectionShader;
	//DebugWindowClass *myDebugWindow;

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