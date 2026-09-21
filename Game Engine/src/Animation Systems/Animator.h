#pragma once
#include <glm/glm.hpp>

class DeltaTime;

namespace IcePick {
	class Skeleton;
	class AnimationLoader;
	struct AnimatorComponent;

	class Animator {
	public:
		void CalculateSkeletonTransforms(Skeleton& skeleton);
		void ProcessAnimationClip(Skeleton& skeleton, AnimatorComponent& animatorComponent, AnimationLoader& animationLoader, DeltaTime dt);
	private:
		void CalculateBoneTransforms(Skeleton& skeleton);
	};
}