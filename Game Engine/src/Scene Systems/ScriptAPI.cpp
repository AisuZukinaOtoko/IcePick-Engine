#include "../Render Systems/Renderer.h"
#include "ScriptAPI.h"
#include "Components.h"
#include "SceneRegistry.h"
#include "SceneCamera.h"
#include "../Public/EngineLayer.h"

namespace ScriptAPI {
	glm::vec3& GetWorldPosition(entt::entity entityId) {
		return IcePick::GetComponent<IcePick::TransformComponent>(entityId).Position;
	}

	glm::quat GetWorldRotation(entt::entity entityId) {
		return IcePick::GetComponent<IcePick::TransformComponent>(entityId).Rotation;
	}

	glm::vec3 GetWorldScale(entt::entity entityId) {
		return IcePick::GetComponent<IcePick::TransformComponent>(entityId).Scale;
	}

	glm::vec2 GetWalkActionAxes() {
		return IcePick::EngineLayer::GameInput.GetWalkActionAxes();
	}

	void DrawLine(glm::vec3 point1, glm::vec3 point2) {
		IcePickRenderer::DrawLine({ point1, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f) }, {point2, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f) });
	}

	void SetWorldPosition(entt::entity entityId, glm::vec3& position) {
		glm::vec3& entityPosition = IcePick::GetComponent<IcePick::TransformComponent>(entityId).Position;
		entityPosition = position;
	}

	void SetWorldRotation(entt::entity entityId, glm::quat rotation) {
		glm::quat& entityRotation = IcePick::GetComponent<IcePick::TransformComponent>(entityId).Rotation;
		entityRotation = rotation;
	}

	void SetWorldScale(entt::entity entityId, glm::vec3 scale) {
		glm::vec3& entityScale = IcePick::GetComponent<IcePick::TransformComponent>(entityId).Scale;
		entityScale = scale;
	}
}