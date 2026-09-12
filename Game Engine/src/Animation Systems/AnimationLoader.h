#pragma once
#include "Timeline.h"
#include "../File Systems/ImportSettings.h"
#include <assimp/scene.h>
#include <unordered_map>

namespace IcePick {
	class Skeleton;

	class AnimationLoader {
	public:
		AnimationLoader();
		~AnimationLoader();
		void ImportAnimationsFromScene(const aiScene* scene, Skeleton& skeleton, ImportSettings importSettings);
		const Timeline& GetTimelineById(UUID timelineId);

		void SerializeAnimation(const SkeletalNodeAnimation& animation, std::filesystem::path animationPath);

		UUID LoadAnimation(std::filesystem::path animationPath);

		SkeletalNodeAnimation& GetSkeletalAnimationById(UUID animationId);

	private:
		SkeletalNodeAnimation m_EmptySkeletalAnimation;
		UUID RegisterSkeletalAnimation(SkeletalNodeAnimation& animation);

		Timeline m_DefaultEmptyTimeline;

		std::unordered_map<UUID, SkeletalNodeAnimation, UUIDHasher> m_LoadedSkeletalAnimations;
		std::unordered_map<UUID, Timeline, UUIDHasher> m_LoadedTimelines;
	};
}