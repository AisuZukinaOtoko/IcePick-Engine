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

		KeyFrame(Type value, float time) : Value(value), KeyTime(time) {}
	};

	template<typename KeyType>
	struct GenericAnimationChannel {
		uint64_t TargetId{ 0 };
		uint64_t TargetData{ 0 };
		std::vector<KeyFrame<KeyType>> ChannelKeys;
	};

	struct TransformChannels {
		GenericAnimationChannel<glm::vec3> PositionChannel;
		GenericAnimationChannel<glm::quat> RotationChannel;
		GenericAnimationChannel<glm::vec3> ScaleChannel;
	};

	struct NodeTransform {
		unsigned int TargetNodeIndex = 0;
		TransformChannels TransformChannels;
	};

	struct SkeletalNodeAnimation {
		UUID AnimationId = UUID::Unitialised();
		UUID Target = UUID::Unitialised();
		std::vector<NodeTransform> NodeTransformChannels;
	};

	struct EntityTransformAnimation {
		entt::entity Target;
		TransformChannels TransformChannels;
	};

	struct GenericAnimation {
		std::vector<GenericAnimationChannel<glm::vec4>> Vec4Channels;
		std::vector<GenericAnimationChannel<glm::vec3>> Vec3Channels;
		std::vector<GenericAnimationChannel<glm::vec2>> Vec2Channels;
		std::vector<GenericAnimationChannel<float>> FloatChannels;
	};

	struct Timeline {
		std::vector<SkeletalNodeAnimation> SkeletalAnimations;
		std::vector<EntityTransformAnimation> TransformAnimations;
		std::vector<GenericAnimation> GenericAnimations;
	};
}