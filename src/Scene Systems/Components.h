#pragma once
#include "glm/glm.hpp"
#include "sol/sol.hpp"
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
		std::vector<MeshNode> Children;
		unsigned int MaterialSlotIndex = (unsigned int)-1;
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
}