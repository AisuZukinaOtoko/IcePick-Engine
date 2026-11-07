#pragma once
#include <glm/glm.hpp>
#include <entt/entt.h>

namespace ScriptAPI {
	// Transforms
	glm::vec3& GetWorldPosition(entt::entity entityId);
	//glm::vec3 GetWorldRotation(entt::entity entityId);
	glm::vec3 GetWorldScale(entt::entity entityId);

	glm::vec2 GetWalkActionAxes();

	void DrawLine(glm::vec3 point1, glm::vec3 point2);

	void SetWorldPosition(entt::entity entityId, glm::vec3& position);
	//void SetWorldRotation(entt::entity entityId, glm::vec3 rotation);
	void SetWorldScale(entt::entity entityId, glm::vec3 scale);
}