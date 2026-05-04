#include "../Public/EngineLayer.h"
#include "ScriptAPI.h"
#include "SceneRegistry.h"
#include "SceneCamera.h"

namespace ScriptAPI {
	glm::vec3& GetWorldPosition(entt::entity entityId) {
		return IcePick::GetComponent<IcePick::TransformComponent>(entityId).Position;
	}

	//glm::vec3 GetWorldRotation(entt::entity entityId) {
	//	return IcePick::GetComponent<IcePick::TransformComponent>(entityId).Rotation;
	//}

	glm::vec3 GetWorldScale(entt::entity entityId) {
		return IcePick::GetComponent<IcePick::TransformComponent>(entityId).Scale;
	}

	glm::vec3 GetSceneCameraFrontVector() {
		auto& sceneRegistry = IcePick::GetActiveSceneRegistry();
		auto sceneCameraView = sceneRegistry.view<IcePick::SceneCamera>();
		entt::entity sceneCameraId = sceneCameraView.front();

		if (sceneCameraId == entt::null)
			return glm::vec3(0.0f);

		IcePick::SceneCamera& sceneCamera = IcePick::GetComponent<IcePick::SceneCamera>(sceneCameraId);
		return sceneCamera.cameraFront;
	}

	glm::vec3 GetSceneCameraRightVector() {
		auto& sceneRegistry = IcePick::GetActiveSceneRegistry();
		auto sceneCameraView = sceneRegistry.view<IcePick::SceneCamera>();
		entt::entity sceneCameraId = sceneCameraView.front();

		if (sceneCameraId == entt::null)
			return glm::vec3(0.0f);

		IcePick::SceneCamera& sceneCamera = IcePick::GetComponent<IcePick::SceneCamera>(sceneCameraId);
		glm::vec3 cameraRight = glm::cross(sceneCamera.cameraFront, sceneCamera.cameraUp);
		return cameraRight;
	}

	glm::vec2 GetWalkActionAxes() {
		return IcePick::EngineLayer::GameInput.GetWalkActionAxes();
	}

	void SetWorldPosition(entt::entity entityId, glm::vec3& position) {
		glm::vec3& entityPosition = IcePick::GetComponent<IcePick::TransformComponent>(entityId).Position;
		entityPosition = position;
	}

	//void SetWorldRotation(entt::entity entityId, glm::vec3 rotation) {
	//	glm::vec3& entityRotation = IcePick::GetComponent<IcePick::TransformComponent>(entityId).Rotation;
	//	entityRotation = rotation;
	//}

	void SetWorldScale(entt::entity entityId, glm::vec3 scale) {
		glm::vec3& entityScale = IcePick::GetComponent<IcePick::TransformComponent>(entityId).Scale;
		entityScale = scale;
	}
}