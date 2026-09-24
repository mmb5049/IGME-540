#include "Transform.h"

using namespace DirectX;

Transform::Transform() :
	position(0,0,0),
	pitchYawRoll(0,0,0),
	scale(1,1,1)
{

}

void Transform::MoveAbsolute(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
	//XMVECTOR p = XMVectorSet(x, y, z, 0);
	//
	//XMVECTOR currentPos = XMLoadFloat3(&position);
	//
	//XMStoreFloat3(
	//	&position,
	//	XMLoadFloat3(&position) + XMVectorSet(x, y, z, 0));
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	pitchYawRoll.x *= pitch;
	pitchYawRoll.y *= yaw;
	pitchYawRoll.z *= roll;
}

void Transform::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
}

void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	pitchYawRoll.x = pitch;
	pitchYawRoll.y = yaw;
	pitchYawRoll.z = roll;
}

void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return DirectX::XMFLOAT3();
}

DirectX::XMFLOAT3 Transform::GetRotation()
{
	return DirectX::XMFLOAT3();
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return DirectX::XMFLOAT3();
}
