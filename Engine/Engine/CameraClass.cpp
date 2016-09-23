#include "CameraClass.h"

CameraClass::CameraClass()
{
	myPositionX = 0;
	myPositionY = 0;
	myPositionZ = 0;

	myRotationX = 0;
	myRotationY = 0;
	myRotationZ = 0;
}

CameraClass::CameraClass(const CameraClass& aOther)
{
}

CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(float aX, float aY, float aZ)
{
	myPositionX = aX;
	myPositionY = aY;
	myPositionZ = aZ;
}

void CameraClass::SetRotation(float aX, float aY, float aZ)
{
	myRotationX = aX;
	myRotationY = aY;
	myRotationZ = aZ;
}

D3DXVECTOR3 CameraClass::GetPosition()
{
	return D3DXVECTOR3(myPositionX, myPositionY, myPositionZ);
}


D3DXVECTOR3 CameraClass::GetRotation()
{
	return D3DXVECTOR3(myRotationX, myRotationY, myRotationZ);
}

float CameraClass::ToRadians(float aValue)
{
	return aValue * 0.0174532925;
}

void CameraClass::Render()
{
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 position(myPositionX, myPositionY, myPositionZ);
	D3DXVECTOR3 lookAt(0.0f, 0.0f, 1.0f);
	float yaw = ToRadians(myRotationX);
	float pitch = ToRadians(myRotationY);
	float roll = ToRadians(myRotationZ);
	D3DXMATRIX rotationMatrix;

	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	lookAt = position + lookAt;
	D3DXMatrixLookAtLH(&myViewMatrix, &position, &lookAt, &up);
}

void CameraClass::GetViewMatrix(D3DXMATRIX& aViewMatrix)
{
	aViewMatrix = myViewMatrix;
}