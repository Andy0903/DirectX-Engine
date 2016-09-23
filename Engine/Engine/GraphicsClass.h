#ifndef GRPAHICSCLASS_H
#define GRAPHICSCLASS_H

#include "Direct3DClass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "colorshaderclass.h"

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

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();

	Direct3DClass *myDirect3D;
	CameraClass* myCamera;
	ModelClass* myModel;
	ColorShaderClass* myColorShader;
};

#endif;