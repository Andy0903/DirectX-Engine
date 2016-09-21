#ifndef INPUTCLASS_H
#define INPUTCLASS_H

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass& aOther);
	~InputClass();

	void Initialize();

	void KeyDown(unsigned int aInput);
	void KeyUp(unsigned int aInput);

	bool IsKeyDown(unsigned int aKey);

private:
	bool myKeysDown[256];
};

#endif