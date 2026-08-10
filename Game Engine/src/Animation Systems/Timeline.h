#pragma once
#include <vector>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <entt/entt.h>
#include "../Scene Systems/UUID.h"

namespace IcePick {

	template<typename Type>
	struct KeyFrame {
		Type Value;
		float KeyTime{ 0.0f };
	};

	template<typename KeyType>
	struct GenericTimelineChannel {
		uint64_t TargetId{ 0 };
		uint64_t TargetData{ 0 };
		std::vector<KeyFrame<KeyType>> ChannelKeys;
	};

	struct SkeletonPose {
		std::vector<glm::mat4> NodeLocalTransforms;
	};

	struct EntityTransformChannel {
		uint32_t EntityId;
		std::vector<KeyFrame<glm::vec3>> PositionKeys;
		std::vector<KeyFrame<glm::quat>> RotationKeys;
		std::vector<KeyFrame<glm::vec3>> ScaleKeys;
	};

	struct SkeletonPoseChannel {
		UUID SkeletonId;
		std::vector<KeyFrame<SkeletonPose>> SkeletalPoseKeys;
	};

	struct Timeline {
		std::vector<EntityTransformChannel> EntityTransformChannels;
		std::vector<SkeletonPoseChannel> SkeletonPoseChannels;
	};
}