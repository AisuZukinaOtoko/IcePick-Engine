#pragma once
#include "entt/entt.h"
#include "Components.h"
#include "../Utilities/Assert.h"

namespace IcePick {
	entt::entity NewEntity();
	entt::entity FindEntityByTag(TagComponent&);
	entt::registry& GetActiveSceneRegistry();

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

	template<typename T>
	void RemoveComponent(entt::entity entity) {
		entt::registry& registry = GetActiveSceneRegistry();
		IP_ASSERT(HasComponent<T>(entity), "No component to remove.");
		registry.erase<T>(entity);
	}


	void DeleteEntity(entt::entity entity);
}
