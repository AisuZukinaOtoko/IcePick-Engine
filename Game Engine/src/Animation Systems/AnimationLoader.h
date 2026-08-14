#pragma once
#include "Timeline.h"
#include "../File Systems/ImportSettings.h"
#include <assimp/scene.h>
#include <unordered_map>

namespace IcePick {
	class AnimationLoader {
	public:
		AnimationLoader();
		~AnimationLoader();
		void ImportAnimationsFromScene(const aiScene* scene, ImportSettings importSettings);
		const Timeline& GetTimelineById(UUID timelineId);

		void SerializeTimeline(const Timeline& timeline, std::filesystem::path timelinePath);
	private:
		Timeline m_DefaultEmptyTimeline;
		std::unordered_map<UUID, Timeline, UUIDHasher> m_LoadedTimelines;
	};
}