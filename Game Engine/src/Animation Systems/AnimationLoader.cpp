#include "AnimationLoader.h"
#include "../LogSystem.h"

namespace IcePick {
	AnimationLoader::AnimationLoader() {

	}

	void AnimationLoader::ImportAnimationsFromScene(aiScene* scene, ImportSettings importSettings) {
		if (!scene) {
			return;
		}

		for (unsigned int i = 0; i < scene->mNumAnimations; i++)
		{
			const aiAnimation* animation = scene->mAnimations[i];

			std::string name = animation->mName.C_Str();
			double duration = animation->mDuration;
			double ticksPerSecond = animation->mTicksPerSecond;
			IP_LOG(name);
			IP_LOG(std::to_string(scene->mNumAnimations) + " animations");
			IP_LOG(std::to_string(duration) + " duration");
			IP_LOG(std::to_string(ticksPerSecond) + " tick speed");
		}
	}

	const Timeline& AnimationLoader::GetTimelineById(UUID timelineId) {
		auto iterator = m_LoadedTimelines.find(timelineId);

		if (iterator == m_LoadedTimelines.end())
			return m_DefaultEmptyTimeline;

		return iterator->second;
	}

	AnimationLoader::~AnimationLoader() {

	}
}