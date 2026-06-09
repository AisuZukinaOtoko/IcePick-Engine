#pragma once
#include <glm/glm.hpp>

namespace IcePickRenderer {
	class VertexLayout;

	struct LinePointVertex3D {
	public:
		LinePointVertex3D();
		LinePointVertex3D(glm::vec3 position, glm::vec4 colour);

		static VertexLayout GetVertexLayout();

		glm::vec4 Colour;
		glm::vec3 Position;
	private:
	};

	struct FullScreenPassVertex {
	public:
		FullScreenPassVertex();
		FullScreenPassVertex(glm::vec2 screenSpaceUV);

		static VertexLayout GetVertexLayout();

		glm::vec2 ScreenSpaceUV;
	private:
	};

	struct StaticVertex3D {
	public:
		StaticVertex3D();
		StaticVertex3D(glm::vec3 position, glm::vec3 normal, glm::vec2 textureCoords);

		static VertexLayout GetVertexLayout();

		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TextureCoords;

	private:

	};

	struct SkeletalVertex3D {
	public:
		SkeletalVertex3D();
		SkeletalVertex3D(glm::vec3 position, glm::vec3 normal, glm::vec2 textureCoords);

		static VertexLayout GetVertexLayout();
		static const unsigned int MaxEffectiveBoneCount = 4; // number of bones that can affect a single vertex

		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TextureCoords;

		int BoneIDs[MaxEffectiveBoneCount];
		float BoneWeights[MaxEffectiveBoneCount];

	private:
		void InitBoneWeights();
	};
}