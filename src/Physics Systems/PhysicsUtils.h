#pragma once
#include <Jolt/Jolt.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace IcePick {
	JPH::RVec3 Vec3ToPhysicsVec3(const glm::vec3& vec3);
	JPH::Quat QuatToPhysicsQuat(const glm::quat& quat);

	glm::vec3 PhysicsVec3ToVec3(const JPH::RVec3& vec3);
	glm::quat PhysicsQuatToQuat(const JPH::Quat& quat);
}