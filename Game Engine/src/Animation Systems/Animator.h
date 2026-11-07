#pragma once
#include <glm/glm.hpp>

class DeltaTime;

namespace IcePick {
	class Skeleton;
	class AnimationLoader;
	struct AnimatorComponent;
	struct IKSolverComponent;

	class Animator {
	public:
		void CalculateSkeletonTransforms(Skeleton& skeleton);
		void SolveIK(Skeleton& skeleton, IKSolverComponent& ikSolverComponent, glm::mat4& entityTransform);
		void ProcessAnimationClip(Skeleton& skeleton, AnimatorComponent& animatorComponent, AnimationLoader& animationLoader, DeltaTime dt);
	private:
		void CalculateBoneTransforms(Skeleton& skeleton);
	};
}