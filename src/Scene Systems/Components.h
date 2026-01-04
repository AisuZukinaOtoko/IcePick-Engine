#pragma once
#include "glm/glm.hpp"
#include <string>
#include <filesystem>
#include <vector>
#include "UUID.h"

namespace IcePick {

	struct IDComponent {
		UUID ID;
	};

	struct MeshComponent {
		unsigned int MeshVertexArrayRegistryIndex = 0;
		int MaterialIndex = -1;
		enum Type { STATIC = 0, ANIMATED } MeshType;
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

	struct MaterialComponent {
		unsigned int MaterialIndex;
	};

	struct SpriteRendererComponent {
		unsigned int Texture;
		glm::ivec4 TextureSourceRect;

		std::filesystem::path TextureFilePath = "";
		bool TextureLoaded = false;
	};

	struct TagComponent {
		std::string value;
	};
	
	struct TransformComponent {
		glm::vec3 Position = glm::vec3(0.0f);
		glm::vec3 Rotation = glm::vec3(0.0f);
		glm::vec3 Scale = glm::vec3(1.0f);
	};

	struct RigidBodyComponent {
		glm::vec3 Velocity;
		float Mass;
	};

	struct LightComponent {
		glm::vec3 PositionOffset;
		glm::vec3 Colour;
		glm::vec3 Direction;
		enum Type { POINT = 0, DIRECTIONAL, SPOT } LightType;
		float Intensity;
	};

	struct SphereColliderComponent {
		glm::vec3 PositionOffset;
		float Radius;
	};

	struct CubeColliderComponent {
		glm::vec3 PositionOffset;
		glm::vec3 RotationOffset;
		float Width;
		float Height;
	};
}