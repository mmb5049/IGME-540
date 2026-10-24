#pragma once

#include "Transform.h"
#include <DirectXMath.h>

class Camera
{
private:
	Transform transform;

	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	float fieldOfView;
	float nearClip;
	float farClip;

	float moveSpeed;
	float mouseLookSpeed;

public:
	Camera(
		float aspectRatio,
		DirectX::XMFLOAT3 position,
		float fieldOfView = DirectX::XM_PIDIV4,
		float nearClip = 0.1f,
		float farClip = 1000.0f,
		float moveSpeed = 5.0f,
		float mouseLookSpeed = 0.002f
	);

	void Update(float dt);

	void UpdateViewMatrix();
	void UpdateProjectionMatrix(float aspectRatio);
	float GetFieldOfView();

	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();

	Transform& GetTransform();
};