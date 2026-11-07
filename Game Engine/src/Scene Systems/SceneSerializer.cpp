#include "SceneSerializer.h"
#include "SceneRegistry.h"
#include "../LogSystem.h"
#include <fstream>

namespace IcePick {

	template<>
	void SerializeComponentJSON<AnimatorComponent>(const AnimatorComponent& component, nlohmann::json& outputStream) {
		nlohmann::json componentBody;
		componentBody["animationId"] = static_cast<uint64_t>(component.AnimationId);
		outputStream["animator"] = componentBody;
	}

	template<>
	AnimatorComponent DeserializeComponentJSON(nlohmann::json& componentData) {
		AnimatorComponent returnComponent;
		returnComponent.AnimationId = JsonUtils::GetUint64(componentData, "animationId", UUID::Unitialised());
		return returnComponent;
	}

	template<>
	void SerializeComponentJSON<CameraControllerComponent>(const CameraControllerComponent& component, nlohmann::json& outputStream) {
		nlohmann::json componentBody;
		JsonUtils::WriteVec3(component.Position, componentBody, "position");
		componentBody["followTarget"] = static_cast<uint32_t>(component.FollowTarget);
		componentBody["lookAtTarget"] = static_cast<uint32_t>(component.LookAtTarget);
		componentBody["mode"] = static_cast<uint32_t>(component.Mode);

		outputStream["cameraController"] = componentBody;
	}

	template<>
	CameraControllerComponent DeserializeComponentJSON(nlohmann::json& componentData) {
		CameraControllerComponent returnComponent;
		returnComponent.Position = JsonUtils::GetVec3(componentData, "position", glm::vec3(0.0f));
		returnComponent.FollowTarget = static_cast<entt::entity>(JsonUtils::GetUint32(componentData, "followTarget", entt::null));
		returnComponent.LookAtTarget = static_cast<entt::entity>(JsonUtils::GetUint32(componentData, "lookAtTarget", entt::null));
		returnComponent.Mode = static_cast<CameraControllerComponent::ControllerMode>(JsonUtils::GetUint32(componentData, "mode"));
		return returnComponent;
	}

	template<>
	void SerializeComponentJSON<MeshRendererComponent>(const MeshRendererComponent& component, nlohmann::json& outputStream) {
		nlohmann::json componentBody;
		componentBody["meshDataId"] = static_cast<uint64_t>(component.meshDataId);
		componentBody["meshType"] = static_cast<uint32_t>(component.MeshType);
		componentBody["meshCount"] = component.MeshCount;
		componentBody["meshVisible"] = component.MeshVisible;

		nlohmann::json materialSlotsJsonObject = nlohmann::json::array();
		for (const auto& materialSlot : component.MaterialSlots) {
			materialSlotsJsonObject.push_back(static_cast<uint64_t>(materialSlot));
		}
		componentBody["materialSlots"] = materialSlotsJsonObject;

		outputStream["meshRenderer"] = componentBody;
	}

	template<>
	MeshRendererComponent DeserializeComponentJSON(nlohmann::json& componentData) {
		MeshRendererComponent returnComponent;
		returnComponent.meshDataId = JsonUtils::GetUint64(componentData, "meshDataId", UUID::Unitialised());
		returnComponent.MeshType = static_cast<ImportSettings::MeshType>(JsonUtils::GetUint32(componentData, "meshType"));
		returnComponent.MeshCount = componentData.value("meshCount", 0);
		returnComponent.MeshVisible = componentData.value("meshVisible", true);

		if (componentData.contains("materialSlots") && componentData["materialSlots"].is_array()) {
			nlohmann::json& materialSlots = componentData["materialSlots"];

			for (auto slotIterator = materialSlots.begin(); slotIterator != materialSlots.end(); slotIterator++) {
				returnComponent.MaterialSlots.push_back(slotIterator->get<uint64_t>());
			}
		}

		return returnComponent;
	}

	template<>
	void SerializeComponentJSON<RigidBodyComponent>(const RigidBodyComponent& component, nlohmann::json& outputStream) {
		nlohmann::json componentBody;
		componentBody["motionType"] = static_cast<uint32_t>(component.MotionType);
		componentBody["colliderShapeCount"] = component.ColliderShapeCount;

		outputStream["rigidBody"] = componentBody;
	}

	template<>
	RigidBodyComponent DeserializeComponentJSON(nlohmann::json& componentData) {
		RigidBodyComponent returnComponent;
		returnComponent.MotionType = static_cast<RigidBodyComponent::MotionTypes>(JsonUtils::GetUint32(componentData, "motionType"));
		returnComponent.ColliderShapeCount = componentData.value("colliderShapeCount", 0);
		return returnComponent;
	}

	template<>
	void SerializeComponentJSON<ScriptComponent>(const ScriptComponent& component, nlohmann::json& outputStream) {
		nlohmann::json componentBody;
		componentBody["scriptId"] = static_cast<uint64_t>(component.ScriptId);
		componentBody["active"] = component.Active;
		outputStream["script"] = componentBody;
	}

	template<>
	ScriptComponent DeserializeComponentJSON(nlohmann::json& componentData) {
		ScriptComponent returnComponent;
		returnComponent.ScriptId = JsonUtils::GetUint64(componentData, "scriptId", UUID::Unitialised());
		returnComponent.Active = componentData.value("active", true);
		return returnComponent;
	}

	template<>
	void SerializeComponentJSON<TagComponent>(const TagComponent& component, nlohmann::json& outputStream) {
		nlohmann::json componentBody;
		componentBody["value"] = component.Value;
		componentBody["type"] = static_cast<uint32_t>(component.Type);
		outputStream["tag"] = componentBody;
	}

	template<>
	TagComponent DeserializeComponentJSON(nlohmann::json& componentData) {
		TagComponent returnComponent;
		returnComponent.Value = componentData.value("value", "Unknown");
		returnComponent.Type = static_cast<TagComponent::EntityType>(JsonUtils::GetUint32(componentData, "type"));
		return returnComponent;
	}

	template<>
	void SerializeComponentJSON<TransformComponent>(const TransformComponent& component, nlohmann::json& outputStream) {
		nlohmann::json componentBody;
		JsonUtils::WriteVec3(component.Position, componentBody, "position");
		JsonUtils::WriteQuaternion(component.Rotation, componentBody, "rotation");
		JsonUtils::WriteVec3(component.Scale, componentBody, "scale");
		outputStream["transform"] = componentBody;
	}

	template<>
	TransformComponent DeserializeComponentJSON(nlohmann::json& componentData) {
		TransformComponent returnComponent;
		returnComponent.Position = JsonUtils::GetVec3(componentData, "position");
		returnComponent.Rotation = JsonUtils::GetQuaternion(componentData, "rotation");
		returnComponent.Scale = JsonUtils::GetVec3(componentData, "scale", glm::vec3(1.0f));
		return returnComponent;
	}

	template<>
	void SerializeComponentJSON<VirtualCharacterComponent>(const VirtualCharacterComponent& component, nlohmann::json& outputStream) {
		nlohmann::json componentBody;
		JsonUtils::WriteVec3(component.CapsuleOffset, componentBody, "capsuleOffset");
		componentBody["capsuleHeight"] = component.CapsuleHeight;
		componentBody["capsuleRadius"] = component.CapsuleRadius;

		outputStream["virtualCharacter"] = componentBody;
	}

	template<>
	VirtualCharacterComponent DeserializeComponentJSON(nlohmann::json& componentData) {
		VirtualCharacterComponent returnComponent;
		returnComponent.CapsuleOffset = JsonUtils::GetVec3(componentData, "capsuleOffset");
		returnComponent.CapsuleHeight = componentData.value("capsuleHeight", 1.0f);
		returnComponent.CapsuleRadius = componentData.value("capsuleRadius", 0.5f);
		return returnComponent;
	}

	void SerializeScene(std::filesystem::path assetPath) {
		entt::registry& registry = GetSceneRegistry(SceneRegistryTypes::DEFAULT);
		auto taggedEntityView = registry.view<TagComponent>();

		nlohmann::json outputJson;
		nlohmann::json registryJson = nlohmann::json::array();

		// Panel entity items
		for (auto entity : taggedEntityView) {
			nlohmann::json entityJson;

			SerializeEntityComponentsJSON<
				AnimatorComponent,
				CameraControllerComponent,
				MeshRendererComponent,
				RigidBodyComponent,
				ScriptComponent,
				TagComponent,
				TransformComponent,
				VirtualCharacterComponent
			>(entity, registry, entityJson);

			entityJson["Id"] = static_cast<uint32_t>(entity);
			registryJson.push_back(entityJson);
		}
		outputJson["registry"] = registryJson;
		std::ofstream outFile(assetPath);
		outFile << std::setw(4) << outputJson;
	}

	void DeserializeScene(std::filesystem::path assetPath) {
		std::ifstream inFile(assetPath);

		if (inFile.fail()) {
			IP_LOG("Unable to open scene: " + assetPath.string(), IP_ERROR_LOG);
			return;
		}

		entt::registry& registry = GetSceneRegistry(SceneRegistryTypes::DEFAULT);
		registry.clear();

		nlohmann::json jsonFileStream = nlohmann::json::parse(inFile);

		if (jsonFileStream.contains("registry") && jsonFileStream["registry"].is_array()) {
			nlohmann::json registryJson = jsonFileStream["registry"];

			for (auto entityIterator = registryJson.begin(); entityIterator != registryJson.end(); entityIterator++) {
				entt::entity proposedEntityId = static_cast<entt::entity>(JsonUtils::GetUint32(*entityIterator, "Id", entt::null));
				entt::entity entityId = registry.create(proposedEntityId);

				if (entityIterator->contains("animator")) {
					nlohmann::json componentData = entityIterator->at("animator");
					registry.emplace<AnimatorComponent>(entityId, DeserializeComponentJSON<AnimatorComponent>(componentData));
				}

				if (entityIterator->contains("cameraController")) {
					nlohmann::json componentData = entityIterator->at("cameraController");
					registry.emplace<CameraControllerComponent>(entityId, DeserializeComponentJSON<CameraControllerComponent>(componentData));
				}

				if (entityIterator->contains("meshRenderer")) {
					nlohmann::json componentData = entityIterator->at("meshRenderer");
					registry.emplace<MeshRendererComponent>(entityId, DeserializeComponentJSON<MeshRendererComponent>(componentData));
				}

				if (entityIterator->contains("rigidBody")) {
					nlohmann::json componentData = entityIterator->at("rigidBody");
					registry.emplace<RigidBodyComponent>(entityId, DeserializeComponentJSON<RigidBodyComponent>(componentData));
				}

				if (entityIterator->contains("script")) {
					nlohmann::json componentData = entityIterator->at("script");
					registry.emplace<ScriptComponent>(entityId, DeserializeComponentJSON<ScriptComponent>(componentData));
				}

				if (entityIterator->contains("tag")) {
					nlohmann::json componentData = entityIterator->at("tag");
					registry.emplace<TagComponent>(entityId, DeserializeComponentJSON<TagComponent>(componentData));
				}

				if (entityIterator->contains("transform")) {
					nlohmann::json componentData = entityIterator->at("transform");
					registry.emplace<TransformComponent>(entityId, DeserializeComponentJSON<TransformComponent>(componentData));
				}

				if (entityIterator->contains("virtualCharacter")) {
					nlohmann::json componentData = entityIterator->at("virtualCharacter");
					registry.emplace<VirtualCharacterComponent>(entityId, DeserializeComponentJSON<VirtualCharacterComponent>(componentData));
				}
			}

		}

	}
}