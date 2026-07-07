#pragma once
#include <string>
#include <glm/glm.hpp>

namespace IcePick {
	struct Bone {
		std::string Name;
		glm::mat4 OffsetMatrix{ 1.0f };
		glm::mat4 FinalTransform{ 1.0f };
	};
}