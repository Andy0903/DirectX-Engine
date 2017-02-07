#include "InputClass.h"

InputClass::InputClass()
{
	myDirectInput = nullptr;
	myKeyboard = nullptr;
	myMouse = nullptr;
}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
}


bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	myScreenWidth = screenWidth;
	myScreenHeight = screenHeight;
	myMouseX = 0;
	myMouseY = 0;

	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&myDirectInput, nullptr);
	if (FAILED(result)) { return false; }

	result = myDirectInput->CreateDevice(GUID_SysKeyboard, &myKeyboard, nullptr);
	if (FAILED(result)) { return false; }

	result = myKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result)) { return false; }

	// Set the cooperative level of the keyboard to not share with other programs. (DISCL_NONEXCLUSEIVE)
	result = myKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result)) { return false; }

	result = myKeyboard->Acquire();
	if (FAILED(result)) { return false; }

	result = myDirectInput->CreateDevice(GUID_SysMouse, &myMouse, nullptr);
	if (FAILED(result)) { return false; }

	result = myMouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result)) { return false; }

	result = myMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result)) { return false; }

	result = myMouse->Acquire();
	if (FAILED(result)) { return false; }

	return true;
}

void InputClass::Shutdown()
{
	if (myMouse)
	{
		myMouse->Unacquire();
		myMouse->Release();
		myMouse = nullptr;
	}

	if (myKeyboard)
	{
		myKeyboard->Unacquire();
		myKeyboard->Release();
		myKeyboard = nullptr;
	}

	if (myDirectInput)
	{
		myDirectInput->Release();
		myDirectInput = nullptr;
	}
}

bool InputClass::Frame()
{
	bool result;

	result = ReadKeyboard();
	if (!result) { return false; }

	result = ReadMouse();
	if (!result) { return false; }

	ProcessInput();

	return true;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;

	result = myKeyboard->GetDeviceState(sizeof(myKeyboardState), (LPVOID)&myKeyboardState);
	if (FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			myKeyboard->Acquire();
		}
		else { return false; }
	}

	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result;
	myPrevMouseState = myMouseState;
	result = myMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&myMouseState);
	if (FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			myMouse->Acquire();
		}
		else { return false; }
	}

	return true;
}

void InputClass::ProcessInput()
{
	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	myMouseX += myMouseState.lX;
	myMouseY += myMouseState.lY;

	// Ensure the mouse location doesn't exceed the screen width or height.
	if (myMouseX < 0) { myMouseX = 0; }
	if (myMouseY < 0) { myMouseY = 0; }
	if (myMouseX > myScreenWidth) { myMouseX = myScreenWidth; }
	if (myMouseY > myScreenHeight) { myMouseY = myScreenHeight; }
}

bool InputClass::IsKeyPressed(int aDIKCode)
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if (myKeyboardState[aDIKCode] & 0x80) { return true; }

	return false;
}

void InputClass::GetMouseLocation(int& aOutX, int& aOutY)
{
	aOutX = myMouseX;
	aOutY = myMouseY;
}

float InputClass::GetMouseDeltaX()
{
	return myMouseState.lX;
}

float InputClass::GetMouseDeltaY()
{
	return myMouseState.lY;
}