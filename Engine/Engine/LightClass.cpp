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

D3DXVECTOR4 LightClass::GetAmbientColor() { return myAmbientColor; }

D3DXVECTOR4 LightClass::GetDiffuseColor() { return myDiffuseColor; }

void LightClass::SetPosition(float aX, float aY, float aZ)
{
	myPosition = D3DXVECTOR3(aX, aY, aZ);
}

void LightClass::SetLookAt(float aX, float aY, float aZ)
{
	myLookAt.x = aX;
	myLookAt.y = aY;
	myLookAt.z = aZ;
}

D3DXVECTOR3 LightClass::GetPosition() { return myPosition; }

void LightClass::GenerateViewMatrix()
{
	D3DXVECTOR3 up{ 0, 1, 0 };
	// Create the view matrix from the three vectors.
	D3DXMatrixLookAtLH(&myViewMatrix, &myPosition, &myLookAt, &up);
}

void LightClass::GenerateProjectionMatrix(float aScreenDepth, float aScreenNear)
{
	float fieldOfView = (float)D3DX_PI / 2.0f;
	float screenAspect = 1.0f;

	// Create the projection matrix for the light.
	D3DXMatrixPerspectiveFovLH(&myProjectionMatrix, fieldOfView, screenAspect, aScreenNear, aScreenDepth);
}

void LightClass::GetViewMatrix(D3DXMATRIX &aViewMatrix)
{
	aViewMatrix = myViewMatrix;
}

void LightClass::GetProjectionMatrix(D3DXMATRIX &aProjectionMatrix)
{
	aProjectionMatrix = myProjectionMatrix;
}

//D3DXVECTOR3 LightClass::GetDirection() { return myDirection; }
//
//D3DXVECTOR4 LightClass::GetSpecularColor() { return mySpecularColor; }
//
//float LightClass::GetSpecularPower() { return mySpecularPower; }
//void LightClass::SetDirection(float aX, float aY, float aZ)
//{
//	myDirection = D3DXVECTOR3(aX, aY, aZ);
//}
//
//void LightClass::SetSpecularColor(float aRed, float aGreen, float aBlue, float aAlpha)
//{
//	mySpecularColor = D3DXVECTOR4(aRed, aGreen, aBlue, aAlpha);
//}
//
//void LightClass::SetSpecularPower(float aSpeclarPower)
//{
//	mySpecularPower = aSpeclarPower;
//}