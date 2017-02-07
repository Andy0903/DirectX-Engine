#include "positionclass.h"

PositionClass::PositionClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}


PositionClass::PositionClass(const PositionClass& aOther)
{
}


PositionClass::~PositionClass()
{
}


void PositionClass::SetPosition(float aX, float aY, float aZ)
{
	m_positionX = aX;
	m_positionY = aY;
	m_positionZ = aZ;
}


void PositionClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}


void PositionClass::GetPosition(float& aX, float& aY, float& aZ)
{
	aX = m_positionX;
	aY = m_positionY;
	aZ = m_positionZ;
}


void PositionClass::GetRotation(float& aX, float& aY, float& aZ)
{
	aX = m_rotationX;
	aY = m_rotationY;
	aZ = m_rotationZ;
}

void PositionClass::MoveForward(float aDt)
{
	float radians = m_rotationY * 0.0174532925f;

	m_positionX += sinf(radians) * mySpeed * aDt;
	m_positionZ += cosf(radians) * mySpeed * aDt;
}


void PositionClass::MoveBackward(float aDt)
{
	float radians =  m_rotationY * 0.0174532925f;

	m_positionX -= sinf(radians) * mySpeed * aDt;
	m_positionZ -= cosf(radians) * mySpeed * aDt;
}

void PositionClass::MoveLeft(float aDt)
{
	float radians = m_rotationY * 0.0174532925f;

	m_positionX -= sinf(radians + M_PI / 2) * mySpeed * aDt;
	m_positionZ -= cosf(radians + M_PI / 2) * mySpeed * aDt;
}

void PositionClass::MoveRight(float aDt)
{
	float radians = m_rotationY * 0.0174532925f;

	m_positionX -= sinf(radians - M_PI / 2) * mySpeed * aDt;
	m_positionZ -= cosf(radians - M_PI / 2) * mySpeed * aDt;
}


void PositionClass::MoveUpward(float aDt)
{
	m_positionY += mySpeed * aDt;
}

void PositionClass::MoveDownward(float aDt)
{
	m_positionY -= mySpeed * aDt;
}

void PositionClass::TurnLeft(float aDt)
{
	m_rotationY -= myRotSpeed * aDt;
	if (m_rotationY < 0.0f) { m_rotationY += 360.0f; }
}


void PositionClass::TurnRight(float aDt)
{
	m_rotationY += myRotSpeed * aDt;
	if (m_rotationY > 360.0f) { m_rotationY -= 360.0f; }
}


void PositionClass::LookUpward(float aDt)
{
	m_rotationX -= myRotSpeed * aDt;
	if (m_rotationX > 90.0f) { m_rotationX = 90.0f; }
}


void PositionClass::LookDownward(float aDt)
{
	m_rotationX += myRotSpeed * aDt;
	if (m_rotationX < -90.0f) { m_rotationX = -90.0f; }
}