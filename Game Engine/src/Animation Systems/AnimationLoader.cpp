#include "AnimationLoader.h"
#include "../File Systems/AssetTypes.h"
#include "../LogSystem.h"
#include <fstream>

namespace IcePick {
	AnimationLoader::AnimationLoader() {

	}

	void AnimationLoader::ImportAnimationsFromScene(const aiScene* scene, ImportSettings importSettings) {
		if (!scene || !importSettings.LoadAnimations) {
			return;
		}

		for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
			Timeline importTimeline;

			const aiAnimation* animation = scene->mAnimations[i];

			std::string name = animation->mName.C_Str();
			double duration = animation->mDuration;
			double ticksPerSecond = animation->mTicksPerSecond;
			IP_LOG(name);
			IP_LOG(std::to_string(scene->mNumAnimations) + " animations");
			IP_LOG(std::to_string(duration) + " duration");
			IP_LOG(std::to_string(ticksPerSecond) + " tick speed");

			std::filesystem::path animationAssetPath = importSettings.ImportTargetLocation / "Animations" / std::string(name + GetAssetTypeExtension(AssetTypes::ANIMATION));
			SerializeTimeline(importTimeline, animationAssetPath);
		}
	}

	const Timeline& AnimationLoader::GetTimelineById(UUID timelineId) {
		auto iterator = m_LoadedTimelines.find(timelineId);

		if (iterator == m_LoadedTimelines.end())
			return m_DefaultEmptyTimeline;

		return iterator->second;
	}

	void AnimationLoader::SerializeTimeline(const Timeline& timeline, std::filesystem::path timelinePath) {
		std::error_code errorCode;
		std::filesystem::create_directories(timelinePath.parent_path(), errorCode);

		if (errorCode) {
			IP_LOG("Could not save timeline asset: " + errorCode.message(), IP_ERROR_LOG);
			return;
		}

		std::ofstream outFile(timelinePath);

		if (!outFile.is_open()) {
			IP_LOG("Failed to save timeline asset. Could not open file: " + timelinePath.string(), IP_ERROR_LOG);
			return;
		}

		outFile << "Temp data.";
	}

	AnimationLoader::~AnimationLoader() {

	}
}