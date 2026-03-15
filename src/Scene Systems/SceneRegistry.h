#pragma once
#include "entt/entt.h"
#include "Components.h"
#include "../Utilities/Assert.h"

namespace IcePick {
	enum class SceneRegistryTypes {
		DEFAULT = 0,
		TEMPORARY
	};
	entt::entity NewEntity();
	entt::entity NewCameraController();
	entt::entity NewPointLight();
	entt::entity NewDirectionalLight();
	entt::entity NewTerrain();

	entt::entity AddSceneCamera();

	entt::entity FindEntityByTag(TagComponent&);
	entt::registry& GetActiveSceneRegistry();
	entt::registry& GetSceneRegistry(SceneRegistryTypes registryType);
	void SetActiveSceneRegistry(SceneRegistryTypes registryType);
	void DuplicateSceneRegistry(entt::registry& sourceRegistry, entt::registry& targetRegistry);

	template<typename T>
	bool HasComponent(entt::entity entity) {
		entt::registry& registry = GetActiveSceneRegistry();
		return registry.all_of<T>(entity);
	}

	template<typename T>
	void AddComponent(entt::entity entity, T component = T{}) {
		entt::registry& registry = GetActiveSceneRegistry();
		IP_ASSERT(!HasComponent<T>(entity), "Attempted to add an existing component.");
		registry.emplace<T>(entity, component);
	}

	template<typename T>
	T& GetComponent(entt::entity entity) {
		entt::registry& registry = GetActiveSceneRegistry();
		IP_ASSERT(HasComponent<T>(entity), "No component to get.");
		return registry.get<T>(entity);
	}

	template <typename... Component>
	entt::entity DuplicateEntity(entt::entity sourceEntity) {
		entt::registry& activeRegistry = GetActiveSceneRegistry();
		if (!activeRegistry.valid(sourceEntity))
			return entt::null;

		entt::entity duplicatedEntity = activeRegistry.create();
		(
			[&] {
				if (HasComponent<Component>(sourceEntity)) {
					AddComponent<Component>(duplicatedEntity, GetComponent<Component>(sourceEntity));
				}
			}
		(), ...);

		return duplicatedEntity;
	}

	template<typename T>
	void RemoveComponent(entt::entity entity) {
		entt::registry& registry = GetActiveSceneRegistry();
		IP_ASSERT(HasComponent<T>(entity), "No component to remove.");
		registry.erase<T>(entity);
	}


	void DeleteEntity(entt::entity entity);
}
