#include "ScriptRunner.h"
#include "../LogSystem.h"
#include "glm/glm.hpp"
#include <fstream>

namespace IcePick {

	void ScriptRunner::Init() {
		m_LuaState.open_libraries(sol::lib::base);
		m_LuaState.set_function("Log", IP_LOG);

		m_LuaState.new_usertype<glm::vec3>(
			"vec3",
			sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
			"x", &glm::vec3::x,
			"y", &glm::vec3::y,
			"z", &glm::vec3::z,

			sol::meta_function::addition, sol::overload([](const glm::vec3& v1, const glm::vec3& v2) { return v1 + v2; })
			);
	}

	ScriptComponent ScriptRunner::CreateScriptComponentFromFile(const std::filesystem::path& scriptPath, entt::entity entityId) {
		auto scriptIterator = m_LoadedScriptComponents.find(scriptPath);
		if (scriptIterator != m_LoadedScriptComponents.end()) {
			ScriptComponent& savedScriptComponent = scriptIterator->second;

			ScriptComponent returnScriptComponent = savedScriptComponent;
			returnScriptComponent.Self = m_LuaState.create_table();
			returnScriptComponent.Self["Id"] = (uint32_t)entityId;

			if (returnScriptComponent.OnCreateFunction.valid()) {
				sol::protected_function_result result = returnScriptComponent.OnCreateFunction(returnScriptComponent.Self);

				if (!result.valid()) {
					sol::error err = result;
					IP_LOG(err.what(), IP_ERROR_LOG);
					returnScriptComponent.Active = false;
				}
			}

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
		returnScriptComponent.Self["Id"] = (uint32_t)entityId;

		returnScriptComponent.OnCreateFunction = componentEnvironment["OnCreate"];
		returnScriptComponent.OnUpdateFunction = componentEnvironment["OnUpdate"];
		returnScriptComponent.OnDestroyFunction = componentEnvironment["OnDestroy"];
		returnScriptComponent.IsValid = true;

		if (returnScriptComponent.OnCreateFunction.valid()) {
			sol::protected_function_result result = returnScriptComponent.OnCreateFunction(returnScriptComponent.Self);

			if (!result.valid()) {
				sol::error err = result;
				IP_LOG(err.what(), IP_ERROR_LOG);
				returnScriptComponent.Active = false;
			}
		}

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