#pragma once
#include "glm/glm.hpp"

namespace IcePick {
	struct MeshComponent {
		unsigned int MeshVertexArrayRegistryIndex;
		enum Type {	STATIC = 0, DEFORM	} MeshType;
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