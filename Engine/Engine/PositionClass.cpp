////////////////////////////////////////////////////////////////////////////////
// Filename: positionclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "positionclass.h"

#define _USE_MATH_DEFINES
#include <math.h>

PositionClass::PositionClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}


PositionClass::PositionClass(const PositionClass& other)
{
}


PositionClass::~PositionClass()
{
}


void PositionClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
}


void PositionClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}


void PositionClass::GetPosition(float& x, float& y, float& z)
{
	x = m_positionX;
	y = m_positionY;
	z = m_positionZ;
}


void PositionClass::GetRotation(float& x, float& y, float& z)
{
	x = m_rotationX;
	y = m_rotationY;
	z = m_rotationZ;
}

void PositionClass::MoveForward(float aDt)
{
	float radians;
	// Convert degrees to radians.
	radians = m_rotationY * 0.0174532925f;

	// Update the position.
	m_positionX += sinf(radians) * mySpeed * aDt;
	m_positionZ += cosf(radians) * mySpeed * aDt;
}


void PositionClass::MoveBackward(float aDt)
{
	float radians;
	// Convert degrees to radians.
	radians = m_rotationY * 0.0174532925f;

	// Update the position.
	m_positionX -= sinf(radians) * mySpeed * aDt;
	m_positionZ -= cosf(radians) * mySpeed * aDt;
}

void PositionClass::MoveLeft(float aDt)
{
	float radians;
	// Convert degrees to radians.
	radians = m_rotationY * 0.0174532925f;

	// Update the position.
	m_positionX -= sinf(radians + M_PI / 2) * mySpeed * aDt;
	m_positionZ -= cosf(radians + M_PI / 2) * mySpeed * aDt;
}

void PositionClass::MoveRight(float aDt)
{
	float radians;
	// Convert degrees to radians.
	radians = m_rotationY * 0.0174532925f;

	// Update the position.
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