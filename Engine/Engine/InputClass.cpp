#include "InputClass.h"

InputClass::InputClass()
{
}

InputClass::InputClass(const InputClass& aOther)
{
}

InputClass::~InputClass()
{
}

void InputClass::Initialize()
{
	for (int i = 0; i < 256; i++)
	{
		myKeysDown[i] = false;
	}
}

void InputClass::KeyDown(unsigned int aInput)
{
	myKeysDown[aInput] = true;
}

void InputClass::KeyUp(unsigned int aInput)
{
	myKeysDown[aInput] = false;
}

bool InputClass::IsKeyDown(unsigned int aKey)
{
	return myKeysDown[aKey];
}