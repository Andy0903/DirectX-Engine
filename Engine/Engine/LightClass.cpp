#include "LightClass.h"

LightClass::LightClass()
{
}

LightClass::LightClass(const LightClass &aOther)
{
}

LightClass::~LightClass()
{
}

void LightClass::SetAmbientColor(float aRed, float aGreen, float aBlue, float aAlpha)
{
	myAmbientColor = D3DXVECTOR4(aRed, aGreen, aBlue, aAlpha);
}


void LightClass::SetDiffuseColor(float aRed, float aGreen, float aBlue, float aAlpha)
{
	myDiffuseColor = D3DXVECTOR4(aRed, aGreen, aBlue, aAlpha);
}

void LightClass::SetDirection(float aX, float aY, float aZ)
{
	myDirection = D3DXVECTOR3(aX, aY, aZ);
}

D3DXVECTOR4 LightClass::GetAmbientColor()
{
	return myAmbientColor;
}

D3DXVECTOR4 LightClass::GetDiffuseColor()
{
	return myDiffuseColor;
}

D3DXVECTOR3 LightClass::GetDirection()
{
	return myDirection;
}
