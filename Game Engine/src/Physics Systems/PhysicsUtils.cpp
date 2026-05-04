#include "PhysicsUtils.h"

namespace IcePick {
	JPH::RVec3 Vec3ToPhysicsVec3(const glm::vec3& vec3) {
		return JPH::RVec3(vec3.x, vec3.y, vec3.z);
	}

	JPH::Quat QuatToPhysicsQuat(const glm::quat& quat) {
		return JPH::Quat(quat.x, quat.y, quat.z, quat.w);
	}

	glm::vec3 PhysicsVec3ToVec3(const JPH::RVec3& vec3) {
		return glm::vec3(vec3.GetX(), vec3.GetY(), vec3.GetZ());
	}

	glm::quat PhysicsQuatToQuat(const JPH::Quat& quat) {
		return glm::quat(quat.GetW(), quat.GetX(), quat.GetY(), quat.GetZ());
	}
}