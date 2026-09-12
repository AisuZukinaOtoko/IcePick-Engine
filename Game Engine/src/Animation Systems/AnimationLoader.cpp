#include "AnimationLoader.h"
#include "Skeleton.h"
#include "../File Systems/AssetTypes.h"
#include "../Scene Systems/AssetRegistry.h"
#include "../LogSystem.h"
#include "../Utilities/JsonUtils.h"
#include <fstream>

constexpr unsigned int ANIMATION_LOADER_VERSION = 1;

namespace IcePick {
	AnimationLoader::AnimationLoader() {

	}

	UUID AnimationLoader::RegisterSkeletalAnimation(SkeletalNodeAnimation& animation) {
		UUID animationId;
		animation.AnimationId = animationId;
		m_LoadedSkeletalAnimations.insert({ animationId, animation });
		return animationId;
	}

	SkeletalNodeAnimation& AnimationLoader::GetSkeletalAnimationById(UUID animationId) {
		auto iterator = m_LoadedSkeletalAnimations.find(animationId);

		if (iterator != m_LoadedSkeletalAnimations.end())
			return iterator->second;

		return m_EmptySkeletalAnimation;
	}

	void AnimationLoader::ImportAnimationsFromScene(const aiScene* scene, Skeleton& skeleton, ImportSettings importSettings) {
		if (!scene || !importSettings.LoadAnimations) {
			return;
		}

		for (unsigned int animationIndex = 0; animationIndex < scene->mNumAnimations; animationIndex++) {
			SkeletalNodeAnimation importAnimation;
			importAnimation.Target = skeleton.Id;

			const aiAnimation* animation = scene->mAnimations[animationIndex];

			std::string name = animation->mName.C_Str();
			double duration = animation->mDuration;
			double ticksPerSecond = animation->mTicksPerSecond;
			IP_LOG(name);
			IP_LOG(std::to_string(scene->mNumAnimations) + " animations");
			IP_LOG(std::to_string(duration) + " duration");
			IP_LOG(std::to_string(ticksPerSecond) + " tick speed");

			for (unsigned int channelIndex = 0; channelIndex < animation->mNumChannels; channelIndex++) {
				aiNodeAnim* channel = animation->mChannels[channelIndex];
				int nodeIndex = skeleton.GetNodeIndex(channel->mNodeName.C_Str());

				if (nodeIndex == -1)
					continue;

				NodeTransform& nodeTransform = importAnimation.NodeTransformChannels.emplace_back();
				nodeTransform.TargetNodeIndex = nodeIndex;

				for (unsigned int positionKeyIndex = 0; positionKeyIndex < channel->mNumPositionKeys; positionKeyIndex++) {
					aiVectorKey& importPositionKey = channel->mPositionKeys[positionKeyIndex];
					nodeTransform.TransformChannels.PositionChannel.ChannelKeys.emplace_back(glm::vec3(importPositionKey.mValue.x, importPositionKey.mValue.y, importPositionKey.mValue.z), importPositionKey.mTime);
				}

				for (unsigned int rotationKeyIndex = 0; rotationKeyIndex < channel->mNumRotationKeys; rotationKeyIndex++) {
					aiQuatKey& importRotationKey = channel->mRotationKeys[rotationKeyIndex];
					nodeTransform.TransformChannels.RotationChannel.ChannelKeys.emplace_back(glm::quat(importRotationKey.mValue.w, importRotationKey.mValue.x, importRotationKey.mValue.y, importRotationKey.mValue.z), importRotationKey.mTime);
				}

				for (unsigned int scaleKeyIndex = 0; scaleKeyIndex < channel->mNumScalingKeys; scaleKeyIndex++) {
					aiVectorKey& importScaleKey = channel->mScalingKeys[scaleKeyIndex];
					nodeTransform.TransformChannels.ScaleChannel.ChannelKeys.emplace_back(glm::vec3(importScaleKey.mValue.x, importScaleKey.mValue.y, importScaleKey.mValue.z), importScaleKey.mTime);
				}
			}
			
			UUID animationId = RegisterSkeletalAnimation(importAnimation);

			std::filesystem::path animationAssetPath = importSettings.ImportTargetLocation / "Animations" / std::string(name + GetAssetTypeExtension(AssetTypes::ANIMATION));
			SerializeAnimation(importAnimation, animationAssetPath);

			AssetRegistry& assetRegistry = GetAssetRegistry();
			AssetRegistryEntry animationAssetEntry{ animationId, animationAssetPath, AssetTypes::ANIMATION };
			assetRegistry.RegisterNewAsset(animationAssetEntry);
		}
	}

	const Timeline& AnimationLoader::GetTimelineById(UUID timelineId) {
		auto iterator = m_LoadedTimelines.find(timelineId);

		if (iterator == m_LoadedTimelines.end())
			return m_DefaultEmptyTimeline;

		return iterator->second;
	}

	UUID AnimationLoader::LoadAnimation(std::filesystem::path animationPath) {
		return UUID::Unitialised();
	}

	void AnimationLoader::SerializeAnimation(const SkeletalNodeAnimation& animation, std::filesystem::path animationPath) {
		std::error_code errorCode;
		std::filesystem::path folderPath = animationPath.parent_path();
		std::filesystem::create_directories(folderPath, errorCode);

		if (errorCode) {
			IP_LOG("Could not save timeline asset: " + errorCode.message(), IP_ERROR_LOG);
			return;
		}

		std::ofstream outFile(animationPath);

		if (!outFile.is_open()) {
			IP_LOG("Failed to save timeline asset. Could not open file: " + animationPath.string(), IP_ERROR_LOG);
			return;
		}

		nlohmann::json json;

		json["version"] = ANIMATION_LOADER_VERSION;
		json["target"] = static_cast<uint64_t>(animation.Target);
		json["Id"] = static_cast<uint64_t>(animation.AnimationId);

		//nlohmann::json animationChannels = nlohmann::json::array();
		//for (const NodeTransform& nodeTransform : animation.NodeTransformChannels) {
		//	nlohmann::json nodeTransformObject;
		//	nodeTransformObject["targetNodeIndex"] = nodeTransform.TargetNodeIndex;

		//	nlohmann::json positionChannelObject;
		//	{
		//		positionChannelObject["targetId"] = nodeTransform.TransformChannels.PositionChannel.TargetId;
		//		positionChannelObject["targetData"] = nodeTransform.TransformChannels.PositionChannel.TargetData;

		//		nlohmann::json positionChannelKeys = nlohmann::json::array();
		//		for (const auto& channelKey : nodeTransform.TransformChannels.PositionChannel.ChannelKeys) {
		//			nlohmann::json positionChannelKeyObject;
		//			positionChannelKeyObject["time"] = channelKey.KeyTime;

		//			nlohmann::json positionChannelKeyValueObject;
		//			positionChannelKeyValueObject["x"] = channelKey.Value.x;
		//			positionChannelKeyValueObject["y"] = channelKey.Value.y;
		//			positionChannelKeyValueObject["z"] = channelKey.Value.z;
		//			positionChannelKeyObject["value"] = positionChannelKeyValueObject;

		//			positionChannelKeys.push_back(positionChannelKeyObject);
		//		}
		//		positionChannelObject["positionChannelKeys"] = positionChannelKeys;
		//	}
		//	nodeTransformObject["positionChannel"] = positionChannelObject;
		//	

		//	animationChannels.push_back(nodeTransformObject);
		//}
		//json["animationChannels"] = animationChannels;

		outFile << json;
	}

	AnimationLoader::~AnimationLoader() {

	}
}