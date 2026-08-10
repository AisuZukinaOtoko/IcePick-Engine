#include "Animator.h"
#include "Skeleton.h"

namespace IcePick {
	void Animator::CalculateSkeletonTransforms(Skeleton& skeleton) {
		glm::mat4 rootTransform{ 1.0f };
		CalculateBoneTransformRecursive(skeleton);
		skeleton.UploadBoneData();
	}

	//void Animator::CalculateBoneTransformRecursive(SkeletonNodeHierarchy& skeletonNode, Skeleton& skeleton, const glm::mat4& parentBoneTransform) {
 //       glm::mat4& localTransform = skeleton.BoneLocalTransforms[skeletonNode.BoneLocalTransformIndex];
 //       glm::mat4 boneWorldTransform = parentBoneTransform * localTransform;
 //       skeleton.BoneParentTransforms[skeletonNode.BoneLocalTransformIndex] = parentBoneTransform;

 //       if (skeletonNode.BoneIndex != -1) {
 //           Bone& bone = skeleton.GetBone(skeletonNode.BoneIndex);
 //           bone.FinalTransform = skeleton.InverseGlobalRootTransform * boneWorldTransform * bone.OffsetMatrix;
 //       }

 //       for (size_t i = 0; i < skeletonNode.Children.size(); i++) {
 //           CalculateBoneTransformRecursive(skeletonNode.Children[i], skeleton, boneWorldTransform);
 //       }
	//}

    void Animator::CalculateBoneTransformRecursive(Skeleton& skeleton) {
        for (size_t i = 0; i < skeleton.Nodes.size(); i++) {
            int parentIndex = skeleton.Nodes[i].ParentNodeIndex;
            
            if (parentIndex == -1) {
                skeleton.NodeGlobalTransforms[i] = skeleton.Nodes[i].LocalTransform;
            }
            else {
                skeleton.NodeGlobalTransforms[i] = skeleton.NodeGlobalTransforms[parentIndex] * skeleton.Nodes[i].LocalTransform;

                if (skeleton.Nodes[i].BoneIndex != -1) {
                    Bone& bone = skeleton.GetBone(skeleton.Nodes[i].BoneIndex);
                    bone.FinalTransform = skeleton.InverseGlobalRootTransform * skeleton.NodeGlobalTransforms[i] * bone.OffsetMatrix;
                }
            }
        }
    }
}