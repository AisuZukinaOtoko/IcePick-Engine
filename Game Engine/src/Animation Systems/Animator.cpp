#include "Animator.h"
#include "Skeleton.h"

namespace IcePick {
	void Animator::CalculateSkeletonTransforms(Skeleton& skeleton) {
		glm::mat4 rootTransform{ 1.0f };
		CalculateBoneTransformRecursive(skeleton.RootBone, skeleton, rootTransform);
		skeleton.UploadBoneData();
	}

	void Animator::CalculateBoneTransformRecursive(SkeletonNodeHierarchy& skeletonNode, Skeleton& skeleton, const glm::mat4& parentBoneTransform) {
		Bone& bone = skeleton.GetBone(skeletonNode.BoneIndex);
		glm::mat4 boneWorldTransform = parentBoneTransform * skeletonNode.BoneLocalTransform;
		bone.FinalTransform = boneWorldTransform * bone.OffsetMatrix;

		for (size_t i = 0; i < skeletonNode.Children.size(); i++) {
			CalculateBoneTransformRecursive(skeletonNode.Children[i], skeleton, boneWorldTransform);
		}
	}
}