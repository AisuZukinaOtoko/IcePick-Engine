#include "Animator.h"
#include "Skeleton.h"

namespace IcePick {
	void Animator::CalculateSkeletonTransforms(Skeleton& skeleton) {
		glm::mat4 rootTransform{ 1.0f };
		CalculateBoneTransformRecursive(skeleton.RootBone, skeleton, rootTransform);
		skeleton.UploadBoneData();
	}

	void Animator::CalculateBoneTransformRecursive(SkeletonNodeHierarchy& skeletonNode, Skeleton& skeleton, const glm::mat4& parentBoneTransform) {
        glm::mat4& localTransform = skeleton.BoneLocalTransforms[skeletonNode.BoneLocalTransformIndex];
        glm::mat4 boneWorldTransform = parentBoneTransform * localTransform;
        skeleton.BoneParentTransforms[skeletonNode.BoneLocalTransformIndex] = parentBoneTransform;

        if (skeletonNode.BoneIndex != -1) {
            Bone& bone = skeleton.GetBone(skeletonNode.BoneIndex);
            bone.FinalTransform = skeleton.InverseGlobalRootTransform * boneWorldTransform * bone.OffsetMatrix;
        }

        for (size_t i = 0; i < skeletonNode.Children.size(); i++) {
            CalculateBoneTransformRecursive(skeletonNode.Children[i], skeleton, boneWorldTransform);
        }
	}
}