#include "ScriptRunner.h"
#include "../LogSystem.h"
#include <fstream>
#include <vector>

namespace IcePick {

	void ScriptRunner::Init() {
		m_LuaState.open_libraries(sol::lib::base);
		m_LuaState.set_function("Log", IP_LOG);		
	}

	ScriptComponent ScriptRunner::CreateScriptComponentFromFile(const std::filesystem::path& scriptPath) {
		auto scriptIterator = m_LoadedScriptComponents.find(scriptPath);
		if (scriptIterator != m_LoadedScriptComponents.end()) {
			ScriptComponent& savedScriptComponent = scriptIterator->second;

			ScriptComponent returnScriptComponent = savedScriptComponent;
			returnScriptComponent.Self = m_LuaState.create_table();
			returnScriptComponent.OnCreateFunction(returnScriptComponent.Self);

			return returnScriptComponent;
		}

		ScriptComponent returnScriptComponent;
		std::string scriptSource = ReadScriptFile(scriptPath);
		sol::load_result scriptChunk = m_LuaState.load(scriptSource);	

		if (!scriptChunk.valid()) {
			IP_LOG("Invalid script.", IP_ERROR_LOG);
			return returnScriptComponent;
		}
		
		sol::environment componentEnvironment = sol::environment(m_LuaState, sol::create, m_LuaState.globals());
		scriptChunk(componentEnvironment);

		returnScriptComponent.ScriptEnvironment = componentEnvironment;
		returnScriptComponent.Self = m_LuaState.create_table();

		returnScriptComponent.OnCreateFunction = componentEnvironment["OnCreate"];
		returnScriptComponent.OnUpdateFunction = componentEnvironment["OnUpdate"];
		returnScriptComponent.OnDestroyFunction = componentEnvironment["OnDestroy"];
		returnScriptComponent.IsValid = true;

		returnScriptComponent.OnCreateFunction(returnScriptComponent.Self);
		m_LoadedScriptComponents.insert({ scriptPath, returnScriptComponent });
		return returnScriptComponent;
	}

	std::string ScriptRunner::ReadScriptFile(const std::filesystem::path& scriptFilePath) {
		std::ifstream file(scriptFilePath);
		if (!file.is_open()) {
			IP_LOG("Failed to load script: " + scriptFilePath.string(), IP_ERROR_LOG);
			return "";
		}

		std::string scriptString((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		return scriptString;
	}

	void ScriptRunner::ShutDown() {
		m_LoadedScriptComponents.clear();
	}
}