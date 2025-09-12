#pragma once
#include "../Scene Systems/UUID.h"

namespace IcePick {
	class SceneLoader {
	public:
		SceneLoader();
		~SceneLoader();
		UUID GetSceneByName(std::string sceneName);

	private:

	};
}