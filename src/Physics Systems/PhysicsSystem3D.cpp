#include "PhysicsSystem3D.h"
#include "PhysicsUtils.h"

#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>

#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/StaticCompoundShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

#include "../LogSystem.h"
#include "../Utilities/Assert.h"

#include <iostream>
#include <cstdarg>
#include <thread>

namespace IcePick {

	void PhysicsSystem3D::Init() {
		JPH::RegisterDefaultAllocator();

		JPH::Factory::sInstance = new JPH::Factory();
		JPH::RegisterTypes();

		m_TempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);
		m_JobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

		// This is the max amount of rigid bodies that you can add to the physics system. If you try to add more you'll get an error.
		const JPH::uint cMaxBodies = 1024; //65536
		// This determines how many mutexes to allocate to protect rigid bodies from concurrent access. Set it to 0 for the default settings.
		const JPH::uint cNumBodyMutexes = 0;

		// This is the max amount of body pairs that can be queued at any time (the broad phase will detect overlapping
		// body pairs based on their bounding boxes and will insert them into a queue for the narrowphase). If you make this buffer
		// too small the queue will fill up and the broad phase jobs will start to do narrow phase work. This is slightly less efficient.
		const JPH::uint cMaxBodyPairs = 1024; //65536

		// This is the maximum size of the contact constraint buffer. If more contacts (collisions between bodies) are detected than this
		// number then these contacts will be ignored and bodies will start interpenetrating / fall through the world.
		const JPH::uint cMaxContactConstraints = 1024; //10240

		// Create mapping table from object layer to broadphase layer
		// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
		// Also have a look at BroadPhaseLayerInterfaceTable or BroadPhaseLayerInterfaceMask for a simpler interface.
		//BPLayerInterfaceImpl broad_phase_layer_interface;

		// Create class that filters object vs broadphase layers
		// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
		// Also have a look at ObjectVsBroadPhaseLayerFilterTable or ObjectVsBroadPhaseLayerFilterMask for a simpler interface.
		//ObjectVsBroadPhaseLayerFilterImpl object_vs_broadphase_layer_filter;

		// Create class that filters object vs object layers
		// Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
		// Also have a look at ObjectLayerPairFilterTable or ObjectLayerPairFilterMask for a simpler interface.
		//ObjectLayerPairFilterImpl object_vs_object_layer_filter;

		// Now we can create the actual physics system.
		//PhysicsSystem physics_system;
		m_PhysicsSystem.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, m_ObjectBroadPhaseLayer, m_ObjectVsBroadPhaseLayerFilter, m_ObjectLayerPairFilter);

		// A body activation listener gets notified when bodies activate and go to sleep
		// Note that this is called from a job so whatever you do here needs to be thread safe.
		// Registering one is entirely optional.
		//MyBodyActivationListener body_activation_listener;
		//physics_system.SetBodyActivationListener(&body_activation_listener);

		// A contact listener gets notified when bodies (are about to) collide, and when they separate again.
		// Note that this is called from a job so whatever you do here needs to be thread safe.
		// Registering one is entirely optional.
		//MyContactListener contact_listener;
		//physics_system.SetContactListener(&contact_listener);


		// We simulate the physics world in discrete time steps. 60 Hz is a good rate to update the physics system.
		const float cDeltaTime = 1.0f / 60.0f;

		// Optional step: Before starting the physics simulation you can optimize the broad phase. This improves collision detection performance (it's pointless here because we only have 2 bodies).
		// You should definitely not call this every frame or when e.g. streaming in a new level section as it is an expensive operation.
		// Instead insert all new objects in batches instead of 1 at a time to keep the broad phase efficient.
		m_PhysicsSystem.OptimizeBroadPhase();
		m_PhysicsSystem.SetGravity(JPH::Vec3(0.0f, -3.81f, 0.0f));

#ifndef DIST
		m_DebugRenderer = new PysiscsDebugRenderer3D();
#endif
	}

	JPH::EActivation PhysicsSystem3D::GetObjectActivation(const RigidBodyComponent& rigidBody) const {
		return JPH::EActivation::DontActivate;
	}

	JPH::EMotionType PhysicsSystem3D::GetObjectMotionType(const RigidBodyComponent& rigidBody) const {
		switch (rigidBody.MotionType) {
		case RigidBodyComponent::MotionTypes::STATIC:
			return JPH::EMotionType::Static;
		case RigidBodyComponent::MotionTypes::DYNAMIC:
			return JPH::EMotionType::Dynamic;
		case RigidBodyComponent::MotionTypes::KINEMATIC:
			return JPH::EMotionType::Kinematic;
		default:
			IP_ASSERT(false, "Invalid object motion type.");
			return JPH::EMotionType::Static;
		}
	}

	glm::vec3 PhysicsSystem3D::GetBodyPosition(JPH::BodyID bodyId) const {
		const JPH::BodyInterface& bodyInterface = m_PhysicsSystem.GetBodyInterface();
		JPH::RVec3 position = bodyInterface.GetPosition(bodyId);
		return PhysicsVec3ToVec3(position);
	}

	glm::quat PhysicsSystem3D::GetBodyRotation(JPH::BodyID bodyId) const {
		const JPH::BodyInterface& bodyInterface = m_PhysicsSystem.GetBodyInterface();
		JPH::Quat rotation = bodyInterface.GetRotation(bodyId);
		return PhysicsQuatToQuat(rotation);
	}


	JPH::BodyID PhysicsSystem3D::AddBoxBody() {
		// The main way to interact with the bodies in the physics system is through the body interface. There is a locking and a non-locking
		// variant of this. We're going to use the locking version (even though we're not planning to access bodies from multiple threads)
		JPH::BodyInterface& bodyInterface = m_PhysicsSystem.GetBodyInterface();

		// Next we can create a rigid body to serve as the floor, we make a large box
		// Create the settings for the collision volume (the shape).
		// Note that for simple shapes (like boxes) you can also directly construct a BoxShape.

		JPH::BoxShapeSettings boxShapeSettings(JPH::Vec3(100.0f, 1.0f, 100.0f));
		//boxShapeSettings.SetEmbedded(); // A ref counted object on the stack (base class RefTarget) should be marked as such to prevent it from being freed when its reference count goes to 0.

		// Create the shape
		JPH::ShapeSettings::ShapeResult boxShapeResult = boxShapeSettings.Create();
		JPH::ShapeRefC boxShape = boxShapeResult.Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()

		// Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.
		JPH::BodyCreationSettings boxSettings(boxShape, JPH::RVec3(0.0, -1.0, 0.0), JPH::Quat::sIdentity(), JPH::EMotionType::Static, ObjectLayers::NON_MOVING);

		// Create the actual rigid body
		JPH::Body* floor = bodyInterface.CreateBody(boxSettings); // Note that if we run out of bodies this can return nullptr

		// Add it to the world
		bodyInterface.AddBody(floor->GetID(), JPH::EActivation::DontActivate);
		return JPH::BodyID();
	}

	JPH::BodyID PhysicsSystem3D::AddSphereBody() {
		// The main way to interact with the bodies in the physics system is through the body interface. There is a locking and a non-locking
		// variant of this. We're going to use the locking version (even though we're not planning to access bodies from multiple threads)
		JPH::BodyInterface& bodyInterface = m_PhysicsSystem.GetBodyInterface();

		// Next we can create a rigid body to serve as the floor, we make a large box
		// Create the settings for the collision volume (the shape).
		// Note that for simple shapes (like boxes) you can also directly construct a BoxShape.
		//JPH::BoxShapeSettings floor_shape_settings(JPH::Vec3(100.0f, 1.0f, 100.0f));
		//floor_shape_settings.SetEmbedded(); // A ref counted object on the stack (base class RefTarget) should be marked as such to prevent it from being freed when its reference count goes to 0.

		// Create the shape
		//JPH::ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
		//JPH::ShapeRefC floor_shape = floor_shape_result.Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()

		// Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.
		//JPH::BodyCreationSettings floor_settings(floor_shape, JPH::RVec3(0.0, -1.0, 0.0), JPH::Quat::sIdentity(), JPH::EMotionType::Static, ObjectLayers::NON_MOVING);

		// Create the actual rigid body
		//JPH::Body* floor = bodyInterface.CreateBody(floor_settings); // Note that if we run out of bodies this can return nullptr

		// Add it to the world
		//bodyInterface.AddBody(floor->GetID(), JPH::EActivation::DontActivate);

		// Now create a dynamic body to bounce on the floor
		// Note that this uses the shorthand version of creating and adding a body to the world
		JPH::BodyCreationSettings sphere_settings(new JPH::SphereShape(0.5f), JPH::RVec3(0.0, 2.0, 0.0), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, ObjectLayers::MOVING);
		JPH::BodyID sphere_id = bodyInterface.CreateAndAddBody(sphere_settings, JPH::EActivation::Activate);

		// Now you can interact with the dynamic body, in this case we're going to give it a velocity.
		// (note that if we had used CreateBody then we could have set the velocity straight on the body before adding it to the physics system)
		bodyInterface.SetLinearVelocity(sphere_id, JPH::Vec3(0.0f, -5.0f, 0.0f));
		return JPH::BodyID();
	}

	JPH::Body* PhysicsSystem3D::PrepareSimpleShapeBody(const TransformComponent& bodyTransform, const RigidBodyComponent& rigidBody) {
		JPH::BodyInterface& bodyInterface = m_PhysicsSystem.GetBodyInterface();

		JPH::RVec3 position = Vec3ToPhysicsVec3(bodyTransform.Position);
		JPH::RVec3 scale = Vec3ToPhysicsVec3(bodyTransform.Scale);
		JPH::Quat rotation = QuatToPhysicsQuat(bodyTransform.Rotation).Normalized();
		JPH::EMotionType motionType = GetObjectMotionType(rigidBody);

		JPH::ShapeSettings::ShapeResult simpleShapeResult = CreateShape(rigidBody.ColliderShapes[0]);
		if (simpleShapeResult.HasError()) {
			IP_LOG("Physics system failed to create the collider shape.", IP_ERROR_LOG);
			IP_LOG(simpleShapeResult.GetError().c_str(), IP_ERROR_LOG);
			return nullptr;
		}

		JPH::ShapeRefC simpleShape = simpleShapeResult.Get();
		JPH::ShapeRefC scaledShape = new JPH::ScaledShape(simpleShape, scale);
		JPH::BodyCreationSettings bodyCreationSettings{ scaledShape, position, rotation, motionType, rigidBody.Layer };

		return bodyInterface.CreateBody(bodyCreationSettings);
	}

	//JPH::Body* PhysicsSystem3D::PrepareBoxShapeBody(const TransformComponent& bodyTransform, const RigidBodyComponent& rigidBody) {
	//	JPH::BodyInterface& bodyInterface = m_PhysicsSystem.GetBodyInterface();

	//	JPH::RVec3 position = Vec3ToPhysicsVec3(bodyTransform.Position);
	//	JPH::RVec3 scale = Vec3ToPhysicsVec3(bodyTransform.Scale);
	//	JPH::Quat rotation = QuatToPhysicsQuat(bodyTransform.Rotation).Normalized();
	//	JPH::EMotionType motionType = GetObjectMotionType(rigidBody);

	//	//JPH::BoxShapeSettings boxShapeSettings{ scale };
	//	JPH::ShapeSettings::ShapeResult boxShapeResult = CreateShape(rigidBody.ColliderShapes[0]);
	//	if (boxShapeResult.HasError()) {
	//		IP_LOG("Physics system failed to create the box collider shape.", IP_ERROR_LOG);
	//		IP_LOG(boxShapeResult.GetError().c_str(), IP_ERROR_LOG);
	//		return nullptr;
	//	}

	//	JPH::ShapeRefC boxShape = boxShapeResult.Get();
	//	JPH::BodyCreationSettings bodyCreationSettings{ boxShape, position, rotation, motionType, rigidBody.Layer };

	//	return bodyInterface.CreateBody(bodyCreationSettings);
	//}

	//JPH::Body* PhysicsSystem3D::PrepareSphereShapeBody(const TransformComponent& bodyTransform, const RigidBodyComponent& rigidBody) {
	//	return nullptr;
	//}

	//JPH::Body* PhysicsSystem3D::PrepareCapsuleShapeBody(const TransformComponent& bodyTransform, const RigidBodyComponent& rigidBody) {
	//	JPH::BodyInterface& bodyInterface = m_PhysicsSystem.GetBodyInterface();

	//	JPH::RVec3 position = Vec3ToPhysicsVec3(bodyTransform.Position);
	//	JPH::RVec3 scale = Vec3ToPhysicsVec3(bodyTransform.Scale);
	//	JPH::Quat rotation = QuatToPhysicsQuat(bodyTransform.Rotation).Normalized();
	//	JPH::EMotionType motionType = GetObjectMotionType(rigidBody);

	//	JPH::CapsuleShapeSettings capsuleShapeSettings{ 0.5f, 0.25f};
	//	JPH::ShapeSettings::ShapeResult capsuleShapeResult = capsuleShapeSettings.Create();
	//	if (capsuleShapeResult.HasError()) {
	//		IP_LOG("Physics system failed to create the capsule collider shape.", IP_ERROR_LOG);
	//		IP_LOG(capsuleShapeResult.GetError().c_str(), IP_ERROR_LOG);
	//		return nullptr;
	//	}

	//	JPH::ShapeRefC boxShape = capsuleShapeResult.Get();
	//	JPH::BodyCreationSettings bodyCreationSettings{ boxShape, position, rotation, motionType, rigidBody.Layer };

	//	return bodyInterface.CreateBody(bodyCreationSettings);
	//}

	JPH::Body* PhysicsSystem3D::PrepareStaticCompoundShapeBody(const TransformComponent& bodyTransform, const RigidBodyComponent& rigidBody) {
		JPH::BodyInterface& bodyInterface = m_PhysicsSystem.GetBodyInterface();

		JPH::RVec3 position = Vec3ToPhysicsVec3(bodyTransform.Position);
		JPH::Vec3 scale = Vec3ToPhysicsVec3(bodyTransform.Scale);
		JPH::Quat rotation = QuatToPhysicsQuat(bodyTransform.Rotation).Normalized();
		JPH::EMotionType motionType = GetObjectMotionType(rigidBody);
		
		JPH::StaticCompoundShapeSettings compoundShapeSettings;
		for (int i = 0; i < rigidBody.ColliderShapeCount; i++) {
			const ColliderShape& colliderShape = rigidBody.ColliderShapes[i];
			JPH::ShapeSettings::ShapeResult shapeResult = CreateShape(colliderShape);

			if (shapeResult.HasError())
				continue;

			JPH::ShapeRefC shape = shapeResult.Get();
			JPH::ShapeRefC scaledShape = new JPH::ScaledShape(shape, scale);
			compoundShapeSettings.AddShape(Vec3ToPhysicsVec3(colliderShape.ColliderOffset), JPH::Quat::sIdentity(), scaledShape);
		}

		JPH::ShapeSettings::ShapeResult compoundShapeResult = compoundShapeSettings.Create();
		if (compoundShapeResult.HasError()) {
			IP_LOG("Physics system failed to create the compound collider shape.", IP_ERROR_LOG);
			IP_LOG(compoundShapeResult.GetError().c_str(), IP_ERROR_LOG);
			return nullptr;
		}

		JPH::ShapeRefC compoundShape = compoundShapeResult.Get();
		JPH::BodyCreationSettings bodyCreationSettings{ compoundShape, position, rotation, motionType, rigidBody.Layer };

		return bodyInterface.CreateBody(bodyCreationSettings);
	}

	JPH::ShapeSettings::ShapeResult PhysicsSystem3D::CreateShape(const ColliderShape& colliderShape) {
		switch (colliderShape.ShapeType) {
		case ColliderShape::ColliderShapeType::BOX_SHAPE:
		{
			JPH::BoxShapeSettings boxShapeSettings{ Vec3ToPhysicsVec3(colliderShape.ColliderScale) };
			return boxShapeSettings.Create();
		}
		case ColliderShape::ColliderShapeType::CAPSULE_SHAPE:
		{
			JPH::CapsuleShapeSettings capsuleShapeSettings{ colliderShape.ColliderScale.y, colliderShape.Radius };
			return capsuleShapeSettings.Create();
		}
		default:
			IP_ASSERT(false, "Invalid shape type.");
			break;
		}
		return JPH::ShapeSettings::ShapeResult();
	}

	void PhysicsSystem3D::MultiCreateAndAddBodyPrepare(const TransformComponent& bodyTransform, RigidBodyComponent& rigidBody) {
		JPH::BodyInterface& bodyInterface = m_PhysicsSystem.GetBodyInterface();
		
		if (rigidBody.ColliderShapeCount == 0) {
			IP_LOG("A rigid body without a collider shape will be ignored by the physics system.", IP_WARN_LOG);
			return;
		}

		JPH::Body* body = nullptr;
		if (rigidBody.ColliderShapeCount > 1) {
			PrepareStaticCompoundShapeBody(bodyTransform, rigidBody);
		}
		else { // 1 collider shape
			const ColliderShape& colliderShape = rigidBody.ColliderShapes[0];
			if (colliderShape.ColliderOffset != glm::vec3(0.0f)) { // if the collider has an offset from the rigid body, we use a compound collider for proper center of mass calculations
				body = PrepareStaticCompoundShapeBody(bodyTransform, rigidBody);
			}
			else {
				body = PrepareSimpleShapeBody(bodyTransform, rigidBody);
				//switch (colliderShape.ShapeType) {
				//case ColliderShape::ColliderShapeType::BOX_SHAPE:
				//	body = PrepareBoxShapeBody(bodyTransform, rigidBody);
				//	break;
				//case ColliderShape::ColliderShapeType::SPHERE_SHAPE:
				//	body = PrepareSphereShapeBody(bodyTransform, rigidBody);
				//	break;
				//case ColliderShape::ColliderShapeType::CAPSULE_SHAPE:
				//	body = PrepareCapsuleShapeBody(bodyTransform, rigidBody);
				//	break;
				//default:
				//	IP_LOG("Invalid collider shape type.", IP_ERROR_LOG);
				//	break;
				//}
			}			
		}

		if (!body) {
			IP_LOG("Physics system failed to create rigid body.", IP_ERROR_LOG);
			return;
		}
		
		rigidBody.RigidBodyId = body->GetID();
		m_MultiAddBodyBuffer.push_back(body->GetID());

		if (rigidBody.RigidBodyId.IsInvalid()) {
			IP_LOG("Body ID invalid", IP_WARN_LOG);
		}
	}

	void PhysicsSystem3D::MultiAddBodiesFinalize() {
		JPH::BodyInterface& bodyInterface = m_PhysicsSystem.GetBodyInterface();
		JPH::BodyInterface::AddState addStateHandle = bodyInterface.AddBodiesPrepare(m_MultiAddBodyBuffer.data(), m_MultiAddBodyBuffer.size());
		bodyInterface.AddBodiesFinalize(m_MultiAddBodyBuffer.data(), m_MultiAddBodyBuffer.size(), addStateHandle, JPH::EActivation::Activate);
		m_MultiAddBodyBuffer.clear();
	}

	void PhysicsSystem3D::Update() {
		const float timeStep = 1.0f / 60.0f;
		m_PhysicsSystem.Update(timeStep, 1,	m_TempAllocator, m_JobSystem);
	}

#ifndef DIST
	void PhysicsSystem3D::DebugRender() {
		JPH::BodyManager::DrawSettings drawSettings;

		drawSettings.mDrawShape = true;
		drawSettings.mDrawBoundingBox = false;
		drawSettings.mDrawCenterOfMassTransform = true;
		drawSettings.mDrawVelocity = false;

		m_PhysicsSystem.DrawBodies(drawSettings, m_DebugRenderer);
	}
#endif

	void PhysicsSystem3D::Shutdown() {
		JPH::UnregisterTypes();

		// Destroy the factory
		delete JPH::Factory::sInstance;
		JPH::Factory::sInstance = nullptr;

		if (m_DebugRenderer) {
			delete m_DebugRenderer;
			JPH::DebugRenderer::sInstance = nullptr;
		}
	}
}