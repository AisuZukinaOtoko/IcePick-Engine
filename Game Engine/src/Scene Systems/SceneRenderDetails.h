#pragma once
#include <glm/glm.hpp>

namespace IcePick {
	struct SceneRenderDetails {
		glm::mat4 ViewProjectionMatrix;
		glm::vec3 CameraPosition;
		float AccumulatedTime = 0.0f;
	};
}