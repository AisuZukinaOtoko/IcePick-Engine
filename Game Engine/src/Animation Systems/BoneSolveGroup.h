#pragma once
#include <string>
#include <glm/glm.hpp>

namespace IcePick {
	struct TwoBoneSolveGroup {
		std::string GroupName;
		glm::vec3 TargetWorldPosition;
		int StartJointNodeIndex = -1;
		int MiddleJointNodeIndex = -1;
		int TargetJointNodeIndex = -1;
	};
}