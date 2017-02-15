#ifndef LIGHTCLASS_H
#define LIGHTCLASS_H

#include <D3DX10math.h>

class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);
	void SetSpecularColor(float, float, float, float);
	//Lower specularPower = greater effect
	void SetSpecularPower(float);

	D3DXVECTOR4 GetAmbientColor();
	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR3 GetDirection();
	D3DXVECTOR4 GetSpecularColor();
	float GetSpecularPower();

	void SetPosition(float, float, float);
	void SetLookAt(float, float, float);
	D3DXVECTOR3 GetPosition();
	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float, float);
	void GetViewMatrix(D3DXMATRIX&);
	void GetProjectionMatrix(D3DXMATRIX&);

private:
	D3DXVECTOR4 myAmbientColor;
	D3DXVECTOR4 myDiffuseColor;
	D3DXVECTOR3 myDirection;
	D3DXVECTOR4 mySpecularColor;
	float mySpecularPower;

	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_lookAt;
	D3DXMATRIX m_viewMatrix;
	D3DXMATRIX m_projectionMatrix;
};

#endif 
