#include "Animator.h"
#include "Skeleton.h"
#include "AnimationLoader.h"
#include "../Scene Systems/Components.h"
#include "../Utilities/Clock.h"
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <math.h>

static glm::quat GlobalRotation(const glm::mat4& m) {
    glm::mat3 r(glm::normalize(glm::vec3(m[0])),
        glm::normalize(glm::vec3(m[1])),
        glm::normalize(glm::vec3(m[2]))); // strips scale
    return glm::normalize(glm::quat_cast(r));
}

static glm::vec3 SafeNormalize(const glm::vec3& v, const glm::vec3& fallback) {
    float len = glm::length(v);
    return len > 1e-5f ? v / len : fallback;
}

namespace IcePick {
	void Animator::CalculateSkeletonTransforms(Skeleton& skeleton) {
		glm::mat4 rootTransform{ 1.0f };
        CalculateBoneTransforms(skeleton);
		skeleton.UploadBoneData();
	}

    void Animator::SolveIK(Skeleton& skeleton, IKSolverComponent& ikSolverComponent, glm::mat4& entityTransform) {
        using namespace glm;

        quat entityRot = GlobalRotation(entityTransform);

        for (auto& twoBoneSolveGroup : ikSolverComponent.TwoBoneSolveGroups) {
            int maxJointNodeIndex = std::max({ twoBoneSolveGroup.StartJointNodeIndex, twoBoneSolveGroup.MiddleJointNodeIndex, twoBoneSolveGroup.TargetJointNodeIndex });
            int minJointNodeIndex = std::min({ twoBoneSolveGroup.StartJointNodeIndex, twoBoneSolveGroup.MiddleJointNodeIndex, twoBoneSolveGroup.TargetJointNodeIndex });
            bool solveGroupIndicesValid = (maxJointNodeIndex < skeleton.Nodes.size()) && (minJointNodeIndex >= 0);

            if (!solveGroupIndicesValid)
                continue;

            mat4& aG = skeleton.NodeGlobalTransforms[twoBoneSolveGroup.StartJointNodeIndex];
            mat4& bG = skeleton.NodeGlobalTransforms[twoBoneSolveGroup.MiddleJointNodeIndex];

            vec3 a = vec3(entityTransform * aG[3]);
            vec3 b = vec3(entityTransform * bG[3]);
            vec3 c = vec3(entityTransform * skeleton.NodeGlobalTransforms[twoBoneSolveGroup.TargetJointNodeIndex][3]);
            vec3 t = twoBoneSolveGroup.TargetWorldPosition;

            float lab = length(b - a);
            float lcb = length(c - b);
            float eps = 0.001f;
            float lat = clamp(length(t - a), std::abs(lab - lcb) + eps, lab + lcb - eps);

            vec3 ac = SafeNormalize(c - a, vec3(0, 0, 1));
            vec3 ab = SafeNormalize(b - a, vec3(0, 1, 0));
            vec3 at = SafeNormalize(t - a, ac);
            vec3 bc = SafeNormalize(c - b, ac);
            vec3 ba = -ab;

            float ac_ab_0 = acos(clamp(dot(ac, ab), -1.f, 1.f));
            float ba_bc_0 = acos(clamp(dot(ba, bc), -1.f, 1.f));
            float ac_at_0 = acos(clamp(dot(ac, at), -1.f, 1.f));

            float ac_ab_1 = acos(clamp((lab * lab + lat * lat - lcb * lcb) / (2 * lab * lat), -1.f, 1.f));
            float ba_bc_1 = acos(clamp((lab * lab + lcb * lcb - lat * lat) / (2 * lab * lcb), -1.f, 1.f));

            // Bend plane normal, with a fallback when the limb is straight.
            // Best fallback: a pole vector or the hinge axis from your rest pose / last frame.
            vec3 poleFallbackAxis = GlobalRotation(skeleton.NodeGlobalTransforms[twoBoneSolveGroup.StartJointNodeIndex]) * vec3(0, 0, 1);
            vec3 axis0 = SafeNormalize(cross(ac, ab), poleFallbackAxis /* e.g. world-space knee axis */);
            vec3 axis1 = SafeNormalize(cross(ac, at), axis0);

            // World rotation of each joint = entityRot * modelSpaceGlobalRot
            quat a_wr = entityRot * GlobalRotation(aG);
            quat b_wr = entityRot * GlobalRotation(bG);
            quat a_inv = inverse(a_wr);
            quat b_inv = inverse(b_wr);

            quat r0 = angleAxis(ac_ab_1 - ac_ab_0, a_inv * axis0);
            quat r1 = angleAxis(ba_bc_1 - ba_bc_0, b_inv * axis0);
            quat r2 = angleAxis(ac_at_0, a_inv * axis1);

            skeleton.Nodes[twoBoneSolveGroup.StartJointNodeIndex].LocalTransform *= toMat4(r2 * r0); // note order
            skeleton.Nodes[twoBoneSolveGroup.MiddleJointNodeIndex].LocalTransform *= toMat4(r1);

            // MINE!!!!!!
            //glm::vec3 startJointPosition = entityTransform * skeleton.NodeGlobalTransforms[twoBoneSolveGroup.StartJointNodeIndex][3];
            //glm::vec3 middleJointPosition = entityTransform * skeleton.NodeGlobalTransforms[twoBoneSolveGroup.MiddleJointNodeIndex][3];
            //glm::vec3 targetJointPosition = entityTransform * skeleton.NodeGlobalTransforms[twoBoneSolveGroup.TargetJointNodeIndex][3];

            //float epsilon = 0.001f;
            //float firstSectionLength = length(middleJointPosition - startJointPosition);
            //float secondSectionLength = length(targetJointPosition - middleJointPosition);
            //float distanceFromStartToTarget = clamp(length(twoBoneSolveGroup.TargetWorldPosition - startJointPosition), epsilon, firstSectionLength + secondSectionLength - epsilon);

            //// Current angles
            //float ac_ab_current = acos(clamp(dot(normalize(targetJointPosition - startJointPosition), normalize(middleJointPosition - startJointPosition)), -1.0f, 1.0f));
            //float ba_bc_current = acos(clamp(dot(normalize(startJointPosition - middleJointPosition), normalize(targetJointPosition - middleJointPosition)), -1.0f, 1.0f));
            //float ac_at_current = acos(clamp(dot(normalize(targetJointPosition - startJointPosition), normalize(twoBoneSolveGroup.TargetWorldPosition - startJointPosition)), -1.0f, 1.0f));

            //// Desired angles
            //float ac_ab_desired = acos(clamp((secondSectionLength * secondSectionLength - firstSectionLength * firstSectionLength - distanceFromStartToTarget * distanceFromStartToTarget) / (-2 * firstSectionLength * distanceFromStartToTarget), -1.0f, 1.0f));
            //float ba_bc_desired = acos(clamp((distanceFromStartToTarget * distanceFromStartToTarget - firstSectionLength * firstSectionLength - secondSectionLength * secondSectionLength) / (-2 * firstSectionLength * secondSectionLength), -1.0f, 1.0f));

            ////vec3 axis0 = normalize(cross(targetJointPosition - startJointPosition, middleJointPosition - startJointPosition));
            //quat a_gr = inverse(GlobalRotation(skeleton.NodeGlobalTransforms[twoBoneSolveGroup.StartJointNodeIndex]));
            //quat b_gr = inverse(GlobalRotation(skeleton.NodeGlobalTransforms[twoBoneSolveGroup.MiddleJointNodeIndex]));

            ////quat r0 = angleAxis(ac_ab_desired - ac_ab_current, angleAxis(a_gr, axis0));
            ////quat r1 = angleAxis(ba_bc_desired - ba_bc_current, angleAxis(b_gr, axis0));

            //vec3 axis0 = normalize(cross(targetJointPosition - startJointPosition, middleJointPosition - startJointPosition));
            //vec3 axis1 = normalize(cross(targetJointPosition - startJointPosition, twoBoneSolveGroup.TargetWorldPosition - startJointPosition));

            //quat r0 = angleAxis(ac_ab_desired - ac_ab_current, (a_gr * axis0));
            //quat r1 = angleAxis(ba_bc_desired - ba_bc_current, (b_gr * axis0));
            //quat r2 = angleAxis(ac_at_current, (inverse(a_gr) * axis1));

            //quat a_lr = GlobalRotation(skeleton.NodeGlobalTransforms[twoBoneSolveGroup.StartJointNodeIndex]);
            //quat b_lr = GlobalRotation(skeleton.NodeGlobalTransforms[twoBoneSolveGroup.MiddleJointNodeIndex]);

            //skeleton.Nodes[twoBoneSolveGroup.StartJointNodeIndex].LocalTransform *= toMat4(r2 * r0);
            //skeleton.Nodes[twoBoneSolveGroup.MiddleJointNodeIndex].LocalTransform *= toMat4(r1);


        }
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
                interpolatedKeyPosition = positionKeys[channelKeyIndex].Value;
                if (channelKeyIndex < positionKeys.size() - 1) {
                    float interpolationFactor = (animatorComponent.Time - positionKeys[channelKeyIndex].KeyTime) / (positionKeys[channelKeyIndex + 1].KeyTime - positionKeys[channelKeyIndex].KeyTime);
                    interpolatedKeyPosition = glm::mix(positionKeys[channelKeyIndex].Value, positionKeys[channelKeyIndex + 1].Value, interpolationFactor);
                }
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

                interpolatedKeyRotation = rotationKeys[channelKeyIndex].Value;
                if (channelKeyIndex < rotationKeys.size() - 1) {
                    float interpolationFactor = (animatorComponent.Time - rotationKeys[channelKeyIndex].KeyTime) / (rotationKeys[channelKeyIndex + 1].KeyTime - rotationKeys[channelKeyIndex].KeyTime);
                    interpolatedKeyRotation = glm::slerp<float>(rotationKeys[channelKeyIndex].Value, rotationKeys[channelKeyIndex + 1].Value, interpolationFactor);
                }
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

                interpolatedKeyScale = scaleKeys[channelKeyIndex].Value;
                if (channelKeyIndex < scaleKeys.size() - 1) {
                    float interpolationFactor = (animatorComponent.Time - scaleKeys[channelKeyIndex].KeyTime) / (scaleKeys[channelKeyIndex + 1].KeyTime - scaleKeys[channelKeyIndex].KeyTime);
                    interpolatedKeyScale = glm::mix(scaleKeys[channelKeyIndex].Value, scaleKeys[channelKeyIndex + 1].Value, interpolationFactor);
                }
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