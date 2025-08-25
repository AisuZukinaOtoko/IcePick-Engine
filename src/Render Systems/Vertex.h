#pragma once
#include "../Vendor/glm/glm.hpp"
#include "VertexLayout.h"

namespace IcePickRenderer {

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

	struct AnimatedVertex3D {
	public:
		AnimatedVertex3D();
		AnimatedVertex3D(glm::vec3 position, glm::vec3 normal, glm::vec2 textureCoords);

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