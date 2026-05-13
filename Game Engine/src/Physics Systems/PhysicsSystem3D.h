#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>

#include "ObjectLayerPairFilterImpl.h"
#include "ObjectBroadPhaseLayerImpl.h"
#include "ObjectVsBroadPhaseLayerImpl.h"

#include "ColliderShapes.h"

#include "PhysicsDebugRenderer.h"

namespace IcePick {
	struct TransformComponent;
	struct RigidBodyComponent;

	class PhysicsSystem3D {
	public:
		void Init();
		void Update();
		void Shutdown();

		void MultiCreateAndAddBodyPrepare(const TransformComponent& bodyTransform, RigidBodyComponent& rigidBody);
		void MultiAddBodiesFinalize();

		void MultiRemoveBodyPrepare(const RigidBodyComponent& rigidBody);
		void MultiRemoveBodiesFinalize();

		glm::vec3 GetBodyPosition(JPH::BodyID bodyId) const;
		glm::quat GetBodyRotation(JPH::BodyID bodyId) const;

		JPH::BodyID AddBoxBody();
		JPH::BodyID AddSphereBody();

#ifndef DIST
		void DebugRender();
#endif
	private:
		JPH::Body* PrepareSimpleShapeBody(const TransformComponent& bodyTransform, const RigidBodyComponent& rigidBody);
		JPH::Body* PrepareStaticCompoundShapeBody(const TransformComponent& bodyTransform, const RigidBodyComponent& rigidBody);

		JPH::Vec3 GetAllowedColliderShapeScale(const JPH::Vec3& bodyTransform, const ColliderShape::ColliderShapeType shapeType);
		JPH::ShapeSettings::ShapeResult CreateShape(const ColliderShape& colliderShape);

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
		std::vector<JPH::BodyID> m_MultiRemoveBodyBuffer;
	};
}