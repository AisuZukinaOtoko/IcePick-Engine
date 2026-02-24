#pragma once
#include "../Vendor/glm/glm.hpp"
#include "../Vendor/entt/entt.h"

namespace ScriptAPI {
	// Transforms
	glm::vec3& GetWorldPosition(entt::entity entityId);
	glm::vec3 GetWorldRotation(entt::entity entityId);
	glm::vec3 GetWorldScale(entt::entity entityId);

	glm::vec3 GetSceneCameraFrontVector();
	glm::vec3 GetSceneCameraRightVector();

	glm::vec2 GetWalkActionAxes();

	void SetWorldPosition(entt::entity entityId, glm::vec3& position);
	void SetWorldRotation(entt::entity entityId, glm::vec3 rotation);
	void SetWorldScale(entt::entity entityId, glm::vec3 scale);
}