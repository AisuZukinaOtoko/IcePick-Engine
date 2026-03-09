#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>

#include "ObjectLayerPairFilterImpl.h"
#include "ObjectBroadPhaseLayerImpl.h"
#include "ObjectVsBroadPhaseLayerImpl.h"

#include "PhysicsDebugRenderer.h"

#include "../Scene Systems/Components.h"

namespace IcePick {
	class PhysicsSystem3D {
	public:
		void Init();
		void Update();
		void Shutdown();

		void MultiCreateAndAddBodyPrepare(const TransformComponent& bodyTransform, RigidBodyComponent& rigidBody);
		void MultiAddBodiesFinalize();

		glm::vec3 GetBodyPosition(JPH::BodyID bodyId) const;
		glm::quat GetBodyRotation(JPH::BodyID bodyId) const;

		JPH::BodyID AddBoxBody();
		JPH::BodyID AddSphereBody();
	private:
		JPH::Body* PrepareBoxShapeBody(const TransformComponent& bodyTransform, const RigidBodyComponent& rigidBody);
		JPH::Body* PrepareSphereShapeBody(const TransformComponent& bodyTransform, const RigidBodyComponent& rigidBody);
		JPH::Body* PrepareCapsuleShapeBody(const TransformComponent& bodyTransform, const RigidBodyComponent& rigidBody);
		JPH::Body* PrepareStaticCompoundShapeBody(const TransformComponent& bodyTransform, const RigidBodyComponent& rigidBody);

		JPH::EActivation GetObjectActivation(const RigidBodyComponent& rigidBody) const;
		JPH::EMotionType GetObjectMotionType(const RigidBodyComponent& rigidBody) const;

		ObjectLayerPairFilterImpl m_ObjectLayerPairFilter;
		ObjectBroadPhaseLayerImpl m_ObjectBroadPhaseLayer;
		ObjectVsBroadPhaseLayerImpl m_ObjectVsBroadPhaseLayerFilter;

		PysiscsDebugRenderer3D* m_DebugRenderer = nullptr;

		JPH::PhysicsSystem m_PhysicsSystem;
		JPH::TempAllocatorImpl* m_TempAllocator = nullptr;
		JPH::JobSystemThreadPool* m_JobSystem = nullptr;

		std::vector<JPH::BodyID> m_MultiAddBodyBuffer;
	};
}