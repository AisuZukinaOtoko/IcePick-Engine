#pragma once
#include <unordered_map>
#include <filesystem>
#include "sol/sol.hpp"
#include "entt/entt.h"
#include "Components.h"

namespace IcePick {
	class ScriptRunner {
	public:
		void Init();
		void ShutDown();
		ScriptComponent CreateScriptComponentFromFile(const std::filesystem::path& scriptPath, entt::entity entityId);
	private:
		std::string ReadScriptFile(const std::filesystem::path& scriptFilePath);
		std::unordered_map<std::filesystem::path, ScriptComponent> m_LoadedScriptComponents;

		sol::state m_LuaState;
	};
} 