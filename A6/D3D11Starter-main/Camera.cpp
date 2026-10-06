#include "Camera.h"
#include "Input.h"

using namespace DirectX;

Camera::Camera(
	float aspectRatio,
	XMFLOAT3 position,
	float fieldOfView,
	float nearClip,
	float farClip,
	float moveSpeed,
	float mouseLookSpeed)
	:
	transform(),
	fieldOfView(fieldOfView),
	nearClip(nearClip),
	farClip(farClip),
	moveSpeed(moveSpeed),
	mouseLookSpeed(mouseLookSpeed)
{
	transform.SetPosition(position.x, position.y, position.z);

	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
}

void Camera::Update(float dt)
{
	// -------------------------
	// Keyboard movement
	// -------------------------

	float movement = moveSpeed * dt;

	// Forward / backward
	if (Input::KeyDown('W'))
	{
		transform.MoveRelative(0.0f, 0.0f, movement);
	}

	if (Input::KeyDown('S'))
	{
		transform.MoveRelative(0.0f, 0.0f, -movement);
	}

	// Strafe left / right
	if (Input::KeyDown('A'))
	{
		transform.MoveRelative(-movement, 0.0f, 0.0f);
	}

	if (Input::KeyDown('D'))
	{
		transform.MoveRelative(movement, 0.0f, 0.0f);
	}

	// Move up / down along world Y axis
	if (Input::KeyDown(VK_SPACE))
	{
		transform.MoveAbsolute(0.0f, movement, 0.0f);
	}

	if (Input::KeyDown('X'))
	{
		transform.MoveAbsolute(0.0f, -movement, 0.0f);
	}

	// -------------------------
	// Mouse rotation
	// -------------------------

	if (Input::MouseLeftDown())
	{
		float mouseX = Input::GetMouseXDelta() * mouseLookSpeed;
		float mouseY = Input::GetMouseYDelta() * mouseLookSpeed;

		// X mouse movement -> yaw
		// Y mouse movement -> pitch
		transform.Rotate(
			mouseY,
			mouseX,
			0.0f
		);

		// Clamp pitch to prevent flipping upside down
		XMFLOAT3 rotation = transform.GetRotation();

		rotation.x = max(
			-XM_PIDIV2,
			min(XM_PIDIV2, rotation.x)
		);

		transform.SetRotation(rotation);
	}

	// Make the view matrix match the updated transform
	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	XMFLOAT3 position = transform.GetPosition();
	XMFLOAT3 rotation = transform.GetRotation();

	// Create a quaternion from the camera's rotation
	XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(
		rotation.x,
		rotation.y,
		rotation.z
	);

	// Camera's default forward direction is +Z
	XMVECTOR forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	// Rotate forward according to camera rotation
	forward = XMVector3Rotate(forward, quaternion);

	// Load camera position
	XMVECTOR cameraPosition = XMLoadFloat3(&position);

	// World up vector
	XMVECTOR worldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// Create view matrix
	XMMATRIX view = XMMatrixLookToLH(
		cameraPosition,
		forward,
		worldUp
	);

	XMStoreFloat4x4(&viewMatrix, view);
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	XMMATRIX projection = XMMatrixPerspectiveFovLH(
		fieldOfView,
		aspectRatio,
		nearClip,
		farClip
	);

	XMStoreFloat4x4(&projectionMatrix, projection);
}

XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

Transform& Camera::GetTransform()
{
	return transform;
}