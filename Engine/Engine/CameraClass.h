#ifndef CAMERACLASS_H
#define CAMERACLASS_H

#include <d3dx10math.h>

class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();

	void Render();
	void GetViewMatrix(D3DXMATRIX&);

private:
	float myPositionX;
	float myPositionY;
	float myPositionZ;

	float myRotationX;
	float myRotationY;
	float myRotationZ;

	D3DXMATRIX myViewMatrix;

	float ToRadians(float);
};

#endif