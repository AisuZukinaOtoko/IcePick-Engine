#pragma once
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <sol/sol.hpp>
#include <entt/entt.h>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include "../Physics Systems/ColliderShapes.h"
#include "../Physics Systems/ObjectAndBroadPhaseLayers.h"
#include "../File Systems/ImportSettings.h"

#include <string>
#include <filesystem>
#include <vector>
#include "UUID.h"

namespace IcePick {

	struct MeshComponent { // Deprecated
		unsigned int MeshVertexArrayRegistryIndex = 0;
		int MaterialIndex = -1;
		enum Type { STATIC = 0, SKINNED } MeshType;
	};

	struct MeshNode {
		glm::mat4 NodeTransform = glm::mat4(1.0f);
		std::vector<unsigned int> VertexArrayIDs; // meshes
		std::vector<unsigned int> MaterialSlotIndices;
		std::vector<MeshNode> Children;
	};

	struct MeshRendererComponent {
		UUID meshDataId = UUID::Unitialised();
		IcePick::ImportSettings::MeshType MeshType{ IcePick::ImportSettings::MeshType::STATIC_MESH };
		//MeshNode RootMeshNode;
		int MeshCount = 0;

		std::filesystem::path MeshFilePath = "";
		std::vector<UUID> MaterialSlots;
		bool MeshLoaded = false;

		bool MeshVisible = true;
		bool CastShadows = false;
		bool ReceiveShadows = false;
	};

	struct IKSolverComponent {
		unsigned int BoneCount = 1;
		unsigned int BoneOneIndex = 0;
		unsigned int BoneTwoIndex = 0;
	};

	struct ScriptComponent {
		UUID ScriptId = UUID::Unitialised();
		sol::table Self = sol::nil;
		bool Active = false;
	};

	struct TagComponent {
		std::string value;
		enum class EntityType {
			ENTITY = 0,
			PREFAB,
			CAMERA,
			CAMERA_CONTROLLER,
			POINT_LIGHT,
			DIRECTIONAL_LIGHT,
			TERRAIN
		} Type = EntityType::ENTITY;
	};
	
	struct TransformComponent {
		glm::vec3 Position = glm::vec3(0.0f);
		glm::quat Rotation = glm::quat(glm::vec3(0.0f));
		glm::vec3 Scale = glm::vec3(1.0f);
	};

	struct PointLightComponent {
		glm::vec3 Colour;
		float Intensity = 1.0f;
	};

	struct DirectionalLightComponent {
		glm::vec3 Colour;
		float Azimuth = 0.0f;
		float Elevation = 0.0f;
		float Intensity = 1.0f;
	};

	struct CameraControllerComponent {
		glm::vec3 Position;

		entt::entity FollowTarget = entt::null;
		entt::entity LookAtTarget = entt::null;

		enum class ControllerMode {
			NONE = 0,
			FOLLOW,
			THIRD_PERSON,
			COUNT
		} Mode = ControllerMode::NONE;

		enum class Interpolation {
			NONE = 0,
			LINEAR,
			COUNT
		} EnterInterpolation = Interpolation::NONE; // when switching to this controller, how should the camera be interpolated

		float InterpolationDuration = 1.0f;
		float Pitch = 0.0f;
		float Yaw = 0.0f;
		float FOV = 45.0f;
	};

	struct RigidBodyComponent {
		static constexpr unsigned int MaxColliderShapeCount = 5;
		ColliderShape ColliderShapes[MaxColliderShapeCount];

		JPH::ObjectLayer Layer = ObjectLayers::NON_MOVING;

		enum class MotionTypes {
			STATIC = 0,
			DYNAMIC,
			KINEMATIC,
			MOTION_TYPE_COUNT
		} MotionType = MotionTypes::STATIC;

		JPH::BodyID RigidBodyId;
		unsigned int ColliderShapeCount = 0;
	};
}