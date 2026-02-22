#pragma once
#include "Components.h"
#include "../Event Systems/Input.h"
#include "../Vendor/glm/glm.hpp"
#include "../Utilities/Clock.h"
#include "../Vendor/entt/entt.h"

namespace IcePick {

	class SceneCamera {
	public:
		SceneCamera();
		glm::mat4 GetViewProjectionMatrix();
		void OnUpdate(Input& inputState, DeltaTime dt);
		void SetNewCameraController(entt::entity cameraControllerId);
		entt::entity GetCameraControllerId();

		glm::vec3 cameraPosition;
		glm::vec3 cameraFront;
		glm::vec3 cameraUp;

		float yaw = 0.0f;
		float pitch = 0.0f;
		float aspectRatio = 1.0f;
		float cameraFOV = 1.0f;

		float cameraNearClip = 0.1f;
		float cameraFarClip = 1000.0f;
	private:
		entt::entity m_CameraControllerId = entt::null;
		void ThirdPersonCameraControllerUpdate(Input& inputState, CameraControllerComponent& cameraController, DeltaTime dt);
	};
}