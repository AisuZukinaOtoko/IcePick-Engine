#pragma once
#include <glm/glm.hpp>

namespace IcePick {
	class Skeleton;
	struct SkeletonNodeHierarchy;

	class Animator {
	public:
		void CalculateSkeletonTransforms(Skeleton& skeleton);
	private:
		void CalculateBoneTransformRecursive(SkeletonNodeHierarchy& skeletonNode, Skeleton& skeleton, const glm::mat4& parentBoneTransform);
	};
}