#ifndef SYSTEMCLASS_H
#define SYSTEMCLASS_H
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include "Inputclass.h"
#include "GraphicsClass.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initalize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame(float);
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

	LPCWSTR myApplicationName;
	HINSTANCE myHInstance;
	HWND myHwnd;

	InputClass* myInput;
	GraphicsClass* myGraphics;

	const int myStandardScreenWidth;
	const int myStandardScreenHeight;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = nullptr;
#endif