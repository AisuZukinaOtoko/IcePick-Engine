#pragma once
#include "glm/glm.hpp"
#include "sol/sol.hpp"
#include "entt/entt.h"
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
		//unsigned int MaterialSlotIndex = (unsigned int)-1;
	};

	struct MeshRendererComponent {
		MeshNode RootMeshNode;
		int MeshCount = 0;

		std::filesystem::path MeshFilePath = "";
		std::vector<UUID> MaterialSlots;
		bool MeshLoaded = false;

		bool MeshVisible = true;
		bool CastShadows = false;
		bool ReceiveShadows = false;
	};

	struct ScriptComponent {
		UUID ScriptId = UUID::Unitialised();
		sol::table Self = sol::nil;
		bool Active = true;
	};

	struct TagComponent {
		std::string value;
		enum class EntityType {
			ENTITY = 0,
			CAMERA_CONTROLLER,
			POINT_LIGHT,
			DIRECTIONAL_LIGHT,
			TERRAIN
		} Type;
	};
	
	struct TransformComponent {
		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec3 Rotation = glm::vec3(0.0f);
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
			FREE_LOOK,
			COUNT
		} Mode;

		enum class Interpolation {
			NONE = 0,
			LINEAR,
			COUNT
		} EnterInterpolation; // when switching to this controller, how should the camera be interpolated

		float InterpolationDuration = 1.0f;
		float Pitch = 0.0f;
		float Yaw = 0.0f;
		float FOV = 45.0f;
	};
}