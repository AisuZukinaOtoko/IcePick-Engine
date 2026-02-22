#include "../Render Systems/Renderer.h"
#include "SceneCamera.h"
#include "SceneRegistry.h"
#include "../Vendor/glm/gtc/matrix_transform.hpp"

namespace IcePick {
	SceneCamera::SceneCamera() {
		cameraPosition = glm::vec3(1.5f, 2.25f, 1.5f);
		cameraFront = glm::vec3(0.0f) - cameraPosition;
		cameraUp = { 0.0f, 1.0f, 0.0f };

		yaw = -135.0f;
		pitch = 35.0f;
		aspectRatio = 1920.0f / 1080.0f;
		cameraFOV = glm::radians(45.0f);
	}

	glm::mat4 SceneCamera::GetViewProjectionMatrix() {
		return glm::perspective(cameraFOV, aspectRatio, cameraNearClip, cameraFarClip) * glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
	}

	void SceneCamera::OnUpdate(Input& inputState, DeltaTime dt) {
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = -sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(direction);

		if (HasComponent<CameraControllerComponent>(m_CameraControllerId)) {
			CameraControllerComponent& currentCameraController = GetComponent<CameraControllerComponent>(m_CameraControllerId);
			if (currentCameraController.Mode == CameraControllerComponent::ControllerMode::THIRD_PERSON) {
				ThirdPersonCameraControllerUpdate(inputState, currentCameraController, dt);
			}
		}
	}

	void SceneCamera::SetNewCameraController(entt::entity cameraControllerId) {
		m_CameraControllerId = cameraControllerId;
	}

	entt::entity SceneCamera::GetCameraControllerId() {
		return m_CameraControllerId;
	}

	void SceneCamera::ThirdPersonCameraControllerUpdate(Input& inputState, CameraControllerComponent& cameraController, DeltaTime dt) {
		if (!HasComponent<TransformComponent>(cameraController.LookAtTarget)) {
			return;
		}

		TransformComponent& lookAtTargetTransform = GetComponent<TransformComponent>(cameraController.LookAtTarget);

		glm::vec2 lookDelta = glm::vec2(0.0f);
		if (IcePickRenderer::IsCursorLocked()) {
			lookDelta = inputState.GetLookActionAxes();
		}

		float mouseSensitivity = 0.01f;
		float viewRadius = 5.0f;

		cameraController.Yaw -= lookDelta.x * mouseSensitivity;
		cameraController.Pitch += lookDelta.y * mouseSensitivity;
		cameraController.Pitch = glm::clamp(cameraController.Pitch, -1.5f, 1.5f);

		glm::vec3& position = cameraController.Position;
		position.x = viewRadius * cos(cameraController.Pitch) * sin(cameraController.Yaw);
		position.y = viewRadius * sin(cameraController.Pitch);
		position.z = viewRadius * cos(cameraController.Pitch) * cos(cameraController.Yaw);
		position += lookAtTargetTransform.Position;

		cameraPosition = cameraController.Position;
		cameraFront = glm::normalize(lookAtTargetTransform.Position - cameraPosition);
	}

}

