#ifndef _POSITIONCLASS_H_
#define _POSITIONCLASS_H_
#define _USE_MATH_DEFINES
#include <math.h>

class PositionClass
{
public:
	PositionClass();
	PositionClass(const PositionClass&);
	~PositionClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	void GetPosition(float&, float&, float&);
	void GetRotation(float&, float&, float&);

	void MoveForward(float);
	void MoveBackward(float);
	void MoveUpward(float);
	void MoveDownward(float);
	void TurnLeft(float);
	void TurnRight(float);
	void LookUpward(float);
	void LookDownward(float);
	void MoveLeft(float);
	void MoveRight(float);

private:
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;

	const float mySpeed = 5;
	const float myRotSpeed = 50;
};

#endif