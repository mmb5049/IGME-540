#include "Transform.h"
using namespace DirectX;

Transform::Transform() :
	position(0,0,0),
	pitchYawRoll(0,0,0),
	scale(1,1,1),
	upVector(0, 1, 0),
	rightVector(1, 0, 0),
	forwardVector(0, 0, 1),
	vectorsMoved(false)
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

void Transform::MoveRelative(float x, float y, float z)
{
	// Create a direction vector from the requested movement
	XMVECTOR direction = XMVectorSet(x, y, z, 0.0f);

	// Create a quaternion representing the current rotation
	XMVECTOR rotation = XMQuaternionRotationRollPitchYaw(
		pitchYawRoll.x,
		pitchYawRoll.y,
		pitchYawRoll.z
	);

	// Rotate the movement direction by the transform's rotation
	XMVECTOR rotatedDirection = XMVector3Rotate(direction, rotation);

	// Add the rotated movement to the current position
	XMVECTOR currentPosition = XMLoadFloat3(&position);
	currentPosition += rotatedDirection;

	// Store the new position
	XMStoreFloat3(&position, currentPosition);
}

void Transform::MoveRelative(XMFLOAT3 offset)
{
	MoveRelative(offset.x, offset.y, offset.z);
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

DirectX::XMFLOAT3 Transform::GetUp()
{
	UpdateVectors();
	return upVector;
}

DirectX::XMFLOAT3 Transform::GetRight()
{
	UpdateVectors();
	return rightVector;
}

DirectX::XMFLOAT3 Transform::GetForward()
{
	UpdateVectors();
	return forwardVector;
}

void Transform::UpdateMatrices()
{
}

void Transform::UpdateVectors()
{
	//Don't need to calculate anything if it hasn't moved
	if (!vectorsMoved)
		return;

	XMVECTOR rotation = XMQuaternionRotationRollPitchYaw(
		pitchYawRoll.x,
		pitchYawRoll.y,
		pitchYawRoll.z
	);

	XMVECTOR calUpVector = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), rotation);
	XMVECTOR calForwardVector = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), rotation);
	XMVECTOR calRightVector = XMVector3Rotate(XMVectorSet(1, 0, 0, 0), rotation);
	XMStoreFloat3(&upVector, calUpVector);
	XMStoreFloat3(&forwardVector, calForwardVector);
	XMStoreFloat3(&rightVector, calRightVector);
}

void Transform::MarkChildTransformsDirty()
{
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