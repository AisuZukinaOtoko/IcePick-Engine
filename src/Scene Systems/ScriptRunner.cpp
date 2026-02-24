#include "ScriptRunner.h"
#include "ScriptAPI.h"
#include "../LogSystem.h"
#include "glm/glm.hpp"
#include <fstream>

namespace IcePick {

	void ScriptRunner::Init() {
		m_LuaState.open_libraries(sol::lib::base, sol::lib::math);
		m_LuaState.set_function("Log", IP_LOG);

		m_LuaState.set_function("GetWorldPosition", ScriptAPI::GetWorldPosition);
		m_LuaState.set_function("GetWorldRotation", ScriptAPI::GetWorldRotation);
		m_LuaState.set_function("GetWorldScale", ScriptAPI::GetWorldScale);

		m_LuaState.set_function("SetWorldPosition", ScriptAPI::SetWorldPosition);
		m_LuaState.set_function("SetWorldRotation", ScriptAPI::SetWorldRotation);
		m_LuaState.set_function("SetWorldScale", ScriptAPI::SetWorldScale);

		m_LuaState.set_function("GetSceneCameraFrontVector", ScriptAPI::GetSceneCameraFrontVector);
		m_LuaState.set_function("GetSceneCameraRightVector", ScriptAPI::GetSceneCameraRightVector);

		m_LuaState.set_function("GetWalkActionAxes", ScriptAPI::GetWalkActionAxes);

		m_LuaState.new_usertype<glm::vec3>(
			"vec3",
			sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
			"x", &glm::vec3::x,
			"y", &glm::vec3::y,
			"z", &glm::vec3::z,

			sol::meta_function::addition, sol::overload([](const glm::vec3& v1, const glm::vec3& v2) { return v1 + v2; }),
			sol::meta_function::multiplication, sol::overload([](const glm::vec3& v1, const float& v2) { return v1 * v2; })
			);

		m_LuaState.new_usertype<glm::vec2>(
			"vec2",
			sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
			"x", &glm::vec2::x,
			"y", &glm::vec2::y,

			sol::meta_function::addition, sol::overload([](const glm::vec2& v1, const glm::vec2& v2) { return v1 + v2; }),
			sol::meta_function::multiplication, sol::overload([](const glm::vec2& v1, const float& v2) { return v1 * v2; })
			);
	}

	UUID ScriptRunner::LoadAndRegisterScript(const std::filesystem::path& scriptPath) {
		auto scriptIterator = m_LoadedScriptPaths.find(scriptPath);
		if (scriptIterator != m_LoadedScriptPaths.end()) {
			return scriptIterator->second;
		}

		std::string scriptSource = ReadScriptFile(scriptPath);
		Script newScript = CreateScript(scriptSource);
		
		if (!newScript.IsValid) {
			return UUID::Unitialised();
		}

		UUID newScriptId;
		m_LoadedScripts.insert({ newScriptId, newScript});
		m_LoadedScriptPaths.insert({ scriptPath, newScriptId });
		m_LoadedScriptToScripPathMap.insert({ newScriptId, scriptPath });

		return newScriptId;
	}

	Script ScriptRunner::CreateScript(const std::string& scriptSource) {
		Script newScript;
		sol::load_result scriptChunk = m_LuaState.load(scriptSource);

		if (!scriptChunk.valid()) {
			IP_LOG("Invalid script.", IP_ERROR_LOG);
			return newScript;
		}

		sol::environment scriptEnvironment = sol::environment(m_LuaState, sol::create, m_LuaState.globals());
		scriptChunk(scriptEnvironment);

		newScript.IsValid = true;
		newScript.ScriptEnvironment = scriptEnvironment;
		newScript.OnCreateFunction = scriptEnvironment["OnCreate"];
		newScript.OnUpdateFunction = scriptEnvironment["OnUpdate"];
		newScript.OnDestroyFunction = scriptEnvironment["OnDestroy"];

		return newScript;
	}

	Script& ScriptRunner::GetScriptById(UUID scriptId) {
		auto scriptIterator = m_LoadedScripts.find(scriptId);
		if (scriptIterator == m_LoadedScripts.end()) {
			return m_EmptyScript;
		}

		return scriptIterator->second;
	}

	void ScriptRunner::ReloadScripts() {
		for (auto& scriptPath : m_LoadedScriptPaths) {
			std::string scriptSource = ReadScriptFile(scriptPath.first);
			Script reloadedScript = CreateScript(scriptSource);

			if (!reloadedScript.IsValid)
				continue;

			m_LoadedScripts.insert_or_assign(scriptPath.second, reloadedScript);
		}
	}

	std::filesystem::path ScriptRunner::GetScriptPath(UUID scriptId) {
		auto scriptPathIterator = m_LoadedScriptToScripPathMap.find(scriptId);
		if (scriptPathIterator != m_LoadedScriptToScripPathMap.end())
			return scriptPathIterator->second;

		return "";
	}

	ScriptComponent ScriptRunner::CreateScriptComponentFromFile(const std::filesystem::path& scriptPath, entt::entity entityId) {
		ScriptComponent returnScriptComponent;
		returnScriptComponent.Active = true;
		returnScriptComponent.ScriptId = LoadAndRegisterScript(scriptPath);
		returnScriptComponent.Self = m_LuaState.create_table();
		returnScriptComponent.Self["Id"] = (uint32_t)entityId;
		returnScriptComponent.Self["temp"] = 0;

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
		m_LoadedScripts.clear();
		m_LoadedScriptPaths.clear();
		m_LoadedScriptToScripPathMap.clear();
	}
}