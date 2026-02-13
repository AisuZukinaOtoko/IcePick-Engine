#include "ScriptAPI.h"
#include "SceneRegistry.h"

namespace ScriptAPI {
	glm::vec3& GetWorldPosition(entt::entity entityId) {
		return IcePick::GetComponent<IcePick::TransformComponent>(entityId).Position;
	}

	glm::vec3 GetWorldRotation(entt::entity entityId) {
		return IcePick::GetComponent<IcePick::TransformComponent>(entityId).Rotation;
	}

	glm::vec3 GetWorldScale(entt::entity entityId) {
		return IcePick::GetComponent<IcePick::TransformComponent>(entityId).Scale;
	}

	void SetWorldPosition(entt::entity entityId, glm::vec3& position) {
		glm::vec3& entityPosition = IcePick::GetComponent<IcePick::TransformComponent>(entityId).Position;
		entityPosition = position;
	}

	void SetWorldRotation(entt::entity entityId, glm::vec3 rotation) {
		glm::vec3& entityRotation = IcePick::GetComponent<IcePick::TransformComponent>(entityId).Rotation;
		entityRotation = rotation;
	}

	void SetWorldScale(entt::entity entityId, glm::vec3 scale) {
		glm::vec3& entityScale = IcePick::GetComponent<IcePick::TransformComponent>(entityId).Scale;
		entityScale = scale;
	}
}