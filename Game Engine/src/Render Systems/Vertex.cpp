#include "Vertex.h"
#include "VertexLayout.h"

using namespace IcePickRenderer;

LinePointVertex3D::LinePointVertex3D() {
	Position = glm::vec3(0.0f);
	Colour = glm::vec4(1.0f);
}

LinePointVertex3D::LinePointVertex3D(glm::vec3 position, glm::vec4 colour) 
	: Position(position), Colour(colour)
{

}

VertexLayout LinePointVertex3D::GetVertexLayout() {
	VertexLayout layout{ sizeof(LinePointVertex3D) };

	layout.Push<float>(4, offsetof(LinePointVertex3D, Colour));
	layout.Push<float>(3, offsetof(LinePointVertex3D, Position));

	return layout;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

FullScreenPassVertex::FullScreenPassVertex() {
	ScreenSpaceUV = glm::vec2(0.0f);
}

FullScreenPassVertex::FullScreenPassVertex(glm::vec2 screenSpaceUV) 
	: ScreenSpaceUV(screenSpaceUV)
{

}

VertexLayout FullScreenPassVertex::GetVertexLayout() {
	VertexLayout layout{ sizeof(FullScreenPassVertex) };

	layout.Push<float>(2, offsetof(FullScreenPassVertex, ScreenSpaceUV));
	return layout;
}


//////////////////////////////////////////////////////////////////////////////////////////////////

StaticVertex3D::StaticVertex3D() {
	Position = glm::vec3(0.0f);
	Normal = glm::vec3(0.0f);
	TextureCoords = glm::vec2(0.0f);
}

StaticVertex3D::StaticVertex3D(glm::vec3 position, glm::vec3 normal, glm::vec2 textureCoords)
	: Position(position), Normal(normal), TextureCoords(textureCoords)
{

}

VertexLayout StaticVertex3D::GetVertexLayout() {
	VertexLayout layout{ sizeof(StaticVertex3D) };

	layout.Push<float>(3, offsetof(StaticVertex3D, Position));
	layout.Push<float>(3, offsetof(StaticVertex3D, Normal));
	layout.Push<float>(2, offsetof(StaticVertex3D, TextureCoords));

	return layout;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void SkinnedVertex3D::InitBoneWeights() {
	for (int i = 0; i < MaxEffectiveBoneCount; i++) {
		BoneIDs[i] = 0;
		BoneWeights[i] = 0.0f;
	}
}

SkinnedVertex3D::SkinnedVertex3D() {
	Position = glm::vec3(0.0f);
	Normal = glm::vec3(0.0f);
	TextureCoords = glm::vec2(0.0f);

	InitBoneWeights();
}

SkinnedVertex3D::SkinnedVertex3D(glm::vec3 position, glm::vec3 normal, glm::vec2 textureCoords)
	: Position(position), Normal(normal), TextureCoords(textureCoords) {
	InitBoneWeights();
}

void SkinnedVertex3D::AddBone(unsigned int boneId, float boneWeight) {
	for (int i = 0; i < MaxEffectiveBoneCount; i++) {
		if (BoneWeights[i] == 0.0f) {
			BoneIDs[i] = boneId;
			BoneWeights[i] = boneWeight;
			return;
		}
	}
}

VertexLayout SkinnedVertex3D::GetVertexLayout() {
	VertexLayout layout{ sizeof(SkinnedVertex3D) };

	layout.Push<float>(3, offsetof(SkinnedVertex3D, Position));
	layout.Push<float>(3, offsetof(SkinnedVertex3D, Normal));
	layout.Push<float>(2, offsetof(SkinnedVertex3D, TextureCoords));
	layout.Push<unsigned int>(MaxEffectiveBoneCount, offsetof(SkinnedVertex3D, BoneIDs));
	layout.Push<float>(MaxEffectiveBoneCount, offsetof(SkinnedVertex3D, BoneWeights));

	return layout;
}