#pragma once
#include <string>
#include <glm/glm.hpp>

namespace IcePick {
	struct TwoBoneSolveGroup {
		std::string GroupName;
		glm::vec3 TargetWorldPosition;
		float Weight = 1.0f;
		int StartJointNodeIndex = -1;
		int MiddleJointNodeIndex = -1;
		int TargetJointNodeIndex = -1;
		bool AlwaysApplySolve = false;
		bool ApplySolve = false; // Engine internal only
	};
}