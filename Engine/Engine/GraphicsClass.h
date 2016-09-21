#ifndef GRPAHICSCLASS_H
#define GRAPHICSCLASS_H

#include <Windows.h>

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass& aOther);
	~GraphicsClass();

	bool Initialize(int aScreenWidth, int aScreenHeight, HWND aHwnd);
	void Shutdown();
	bool Frame();

private:
	bool Render();
};

#endif;