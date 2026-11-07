#pragma once
#include <nlohmann/json.hpp>
#include <filesystem>
#include "Components.h"
#include "../Utilities/JsonUtils.h"

namespace IcePick {
	void SerializeScene(std::filesystem::path assetPath);
	void DeserializeScene(std::filesystem::path assetPath);

	template<typename T>
	void SerializeComponentJSON(const T& component, nlohmann::json& outputStream) {
		assert(false, "Cannot serialze this component.");
	}

	template<typename T>
	T DeserializeComponentJSON(nlohmann::json& componentData) {
		assert(false, "Cannot deserialize this component.");
	}

	//template<>
	//void SerializeComponentJSON<AnimatorComponent>(const AnimatorComponent& component, nlohmann::json& outputStream);

	//template<>
	//void SerializeComponentJSON<CameraControllerComponent>(const CameraControllerComponent& component, nlohmann::json& outputStream);

	//template<>
	//void SerializeComponentJSON<MeshRendererComponent>(const MeshRendererComponent& component, nlohmann::json& outputStream);

	//template<>
	//void SerializeComponentJSON<RigidBodyComponent>(const RigidBodyComponent& component, nlohmann::json& outputStream);

	//template<>
	//void SerializeComponentJSON<ScriptComponent>(const ScriptComponent& component, nlohmann::json& outputStream);

	//template<>
	//void SerializeComponentJSON<TagComponent>(const TagComponent& component, nlohmann::json& outputStream);

	//template<>
	//void SerializeComponentJSON<TransformComponent>(const TransformComponent& component, nlohmann::json& outputStream);

	//template<>
	//void SerializeComponentJSON<VirtualCharacterComponent>(const VirtualCharacterComponent& component, nlohmann::json& outputStream);

	template<typename... Component>
	void SerializeEntityComponentsJSON(entt::entity entity, entt::registry& registry, nlohmann::json& outputStream) {
		(
			[&] {
				if (registry.all_of<Component>(entity)) {
					SerializeComponentJSON(registry.get<Component>(entity), outputStream);
				}
			}
		(), ...);
	}
}