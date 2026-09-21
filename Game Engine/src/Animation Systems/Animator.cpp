#include "Animator.h"
#include "Skeleton.h"
#include "AnimationLoader.h"
#include "../Scene Systems/Components.h"
#include "../Utilities/Clock.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <math.h>

namespace IcePick {
	void Animator::CalculateSkeletonTransforms(Skeleton& skeleton) {
		glm::mat4 rootTransform{ 1.0f };
        CalculateBoneTransforms(skeleton);
		skeleton.UploadBoneData();
	}

    void Animator::ProcessAnimationClip(Skeleton& skeleton, AnimatorComponent& animatorComponent, AnimationLoader& animationLoader, DeltaTime dt) {
        if (animatorComponent.AnimationId == UUID::Unitialised())
            return;

        SkeletalNodeAnimation& skeletalAnimation = animationLoader.GetSkeletalAnimationById(animatorComponent.AnimationId);
        if (skeletalAnimation.Target != skeleton.Id)
            return;

        animatorComponent.Time += dt.GetDelta();
        animatorComponent.Time = fmod(animatorComponent.Time, skeletalAnimation.Duration);

        for (const auto& nodeTransformChannel : skeletalAnimation.NodeTransformChannels) {
            SkeletonNode& skeletonNode = skeleton.Nodes[nodeTransformChannel.TargetNodeIndex];

            glm::mat4 nodeLocalTransform{ 1.0f };

            // Position
            unsigned int channelKeyIndex = 0;
            const auto& positionKeys = nodeTransformChannel.TransformChannels.PositionChannel.ChannelKeys;
            glm::vec3 interpolatedKeyPosition{ 1.0f, 1.0f, 1.0f };
            if (positionKeys.size()) {
                for (size_t i = 0; i < positionKeys.size(); i++) {
                    const auto& channelKey = positionKeys[i];
                    if (channelKey.KeyTime > animatorComponent.Time) {
                        channelKeyIndex = i;
                        break;
                    }
                }
                //float keyTimeRation = (positionKeys[channelKeyIndex] - anim);

                interpolatedKeyPosition = positionKeys[channelKeyIndex].Value;
                //if (channelKeyIndex == 0) {

                //}
                //else {

                //}
            }
            
            // Rotation
            channelKeyIndex = 0;
            const auto& rotationKeys = nodeTransformChannel.TransformChannels.RotationChannel.ChannelKeys;
            glm::quat interpolatedKeyRotation = glm::quat(glm::vec3(0.0f));
            if (rotationKeys.size()) {
                for (size_t i = 0; i < rotationKeys.size(); i++) {
                    const auto& channelKey = rotationKeys[i];
                    if (channelKey.KeyTime > animatorComponent.Time) {
                        channelKeyIndex = i;
                        break;
                    }
                }
                //float keyTimeRation = (positionKeys[channelKeyIndex] - anim);

                interpolatedKeyRotation = rotationKeys[channelKeyIndex].Value;
                //if (channelKeyIndex == 0) {

                //}
                //else {

                //}
            }

            // Scale
            channelKeyIndex = 0;
            const auto& scaleKeys = nodeTransformChannel.TransformChannels.ScaleChannel.ChannelKeys;
            glm::vec3 interpolatedKeyScale{ 1.0f, 1.0f, 1.0f };
            if (scaleKeys.size()) {
                for (size_t i = 0; i < scaleKeys.size(); i++) {
                    const auto& channelKey = scaleKeys[i];
                    if (channelKey.KeyTime > animatorComponent.Time) {
                        channelKeyIndex = i;
                        break;
                    }
                }
                //float keyTimeRation = (positionKeys[channelKeyIndex] - anim);

                interpolatedKeyScale = scaleKeys[channelKeyIndex].Value;
                //if (channelKeyIndex == 0) {

                //}
                //else {

                //}
            }

            nodeLocalTransform = glm::translate(nodeLocalTransform, interpolatedKeyPosition);
            nodeLocalTransform *= glm::toMat4(interpolatedKeyRotation);
            nodeLocalTransform = glm::scale(nodeLocalTransform, interpolatedKeyScale);
            
            skeletonNode.LocalTransform = nodeLocalTransform;
        }
    }

    void Animator::CalculateBoneTransforms(Skeleton& skeleton) {
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