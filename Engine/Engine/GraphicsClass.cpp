#include "GraphicsClass.h"

GraphicsClass::GraphicsClass()
{
}

GraphicsClass::GraphicsClass(const GraphicsClass& aOther)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int aScreenWidth, int aScreenHeight, HWND aHwnd)
{
	return true;
}

void GraphicsClass::Shutdown()
{

}

bool GraphicsClass::Frame()
{
	return true;
}

bool GraphicsClass::Render()
{
	return true;
}