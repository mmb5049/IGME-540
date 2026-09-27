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

void Transform::MoveAbsolute(XMFLOAT3 offset)
{
	MoveAbsolute(offset.x, offset.y, offset.z);
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	pitchYawRoll.x += pitch;
	pitchYawRoll.y += yaw;
	pitchYawRoll.z += roll;
}

void Transform::Rotate(XMFLOAT3 rotation)
{
	Rotate(rotation.x, rotation.y, rotation.z);
}


void Transform::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
}

void Transform::Scale(XMFLOAT3 scale)
{
	Scale(scale.x, scale.y, scale.z);
}


void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void Transform::SetPosition(XMFLOAT3 position)
{
	SetPosition(position.x, position.y, position.z);
}


void Transform::SetRotation(float pitch, float yaw, float roll)
{
	pitchYawRoll.x = pitch;
	pitchYawRoll.y = yaw;
	pitchYawRoll.z = roll;
}

void Transform::SetRotation(XMFLOAT3 rotation)
{
	SetRotation(rotation.x, rotation.y, rotation.z);
}


void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;
}

void Transform::SetScale(XMFLOAT3 scale)
{
	SetScale(scale.x, scale.y, scale.z);
}

XMFLOAT3 Transform::GetPosition()
{
	return position;
}

XMFLOAT3 Transform::GetRotation()
{
	return pitchYawRoll;
}

XMFLOAT3 Transform::GetScale()
{
	return scale;
}


XMFLOAT4X4 Transform::GetWorldMatrix()
{
	XMMATRIX scaleMatrix = XMMatrixScaling(
		scale.x,
		scale.y,
		scale.z
	);

	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(
		pitchYawRoll.x,
		pitchYawRoll.y,
		pitchYawRoll.z
	);

	XMMATRIX translationMatrix = XMMatrixTranslation(
		position.x,
		position.y,
		position.z
	);

	XMMATRIX world = scaleMatrix * rotationMatrix * translationMatrix;
	XMMATRIX inverseTransposeMatrix = XMMatrixInverse(nullptr, XMMatrixTranspose(world));
	XMStoreFloat4x4(&worldMatrix, world);
	XMStoreFloat4x4(&worldInverseTransposeMatrix, inverseTransposeMatrix);

	return worldMatrix;
}