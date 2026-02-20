#include "SceneCamera.h"
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

	void SceneCamera::OnUpdate(DeltaTime dt) {
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = -sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(direction);
	}

	void SceneCamera::SetNewCameraController(entt::entity cameraControllerId) {
		m_CameraControllerId = cameraControllerId;
	}

	entt::entity SceneCamera::GetCameraControllerId() {
		return m_CameraControllerId;
	}

}

