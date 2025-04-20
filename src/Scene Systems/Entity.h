#pragma once
#include <cstdint>
#include "SceneRegistry.h"
#include "../Utilities/Assert.h"


namespace IcePick{
	class UUID {
	public:
		UUID();
		uint64_t value;
	};

	class Entity {
	public:
		Entity() = default;

		void CreateCopy();

		/*template<typename T>
		void AddComponent(T component = T{}) {
			IP_ASSERT(!HasComponent<T>(), "Attempted to add an existing component.");
			IP_SceneRegistry.emplace<T>(m_EntityID, component);
		}

		template<typename T>
		bool HasComponent() {
			return IP_SceneRegistry.all_of<T>(m_EntityID);
		}

		template<typename T>
		T& GetComponent() {
			IP_ASSERT(HasComponent<T>(), "Attempted to get a non-existing component.");
			return IP_SceneRegistry.get<T>(m_EntityID);
		}

		template<typename T>
		void RemoveComponent() {
			IP_ASSERT(HasComponent<T>(), "Attempted to remove a non-existing component.");
			IP_SceneRegistry.remove<T>(m_EntityID);
		}*/

	private:
		uint16_t m_Components = 0;
		UUID m_ID;
		uint32_t m_EntityID;
	};
	 
}