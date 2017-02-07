#ifndef INPUTCLASS_H
#define INPUTCLASS_H

#define DIRECTINPUT_VERSION 0x0800
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	bool IsKeyPressed(int);
	void GetMouseLocation(int&, int&);
	float GetMouseDeltaX();
	float GetMouseDeltaY();

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

	IDirectInput8* myDirectInput;
	IDirectInputDevice8* myKeyboard;
	IDirectInputDevice8* myMouse;

	unsigned char myKeyboardState[256];
	DIMOUSESTATE myMouseState, myPrevMouseState;

	int myScreenWidth, myScreenHeight;
	int myMouseX, myMouseY;
};


#endif