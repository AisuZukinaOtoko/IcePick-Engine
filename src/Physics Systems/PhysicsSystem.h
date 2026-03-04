#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>

namespace IcePick {
	class PhysicsSystem {
	public:
		void Initialise();
		void Update();
		void Shutdown();
	private:
		JPH::PhysicsSystem m_PhysicsSystem;
		JPH::TempAllocatorImpl* m_TempAllocator = nullptr;
		JPH::JobSystemThreadPool* m_JobSystem = nullptr;
	};
}