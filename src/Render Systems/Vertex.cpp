#include "Vertex.h"
using namespace IcePickRenderer;

StaticVertex3D::StaticVertex3D() {
	Position = glm::vec3(0.0f);
	Normal = glm::vec3(0.0f);
	TextureCoords = glm::vec2(0.0f);
}

StaticVertex3D::StaticVertex3D(glm::vec3 position, glm::vec3 normal, glm::vec2 textureCoords)
	: Position(position), Normal(normal), TextureCoords(textureCoords) {

}

VertexLayout StaticVertex3D::GetVertexLayout() {
	VertexLayout layout(sizeof(StaticVertex3D));

	layout.Push<float>(3, offsetof(StaticVertex3D, Position));
	layout.Push<float>(3, offsetof(StaticVertex3D, Normal));
	layout.Push<float>(2, offsetof(StaticVertex3D, TextureCoords));

	return layout;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void AnimatedVertex3D::InitBoneWeights() {
	for (int i = 0; i < MaxEffectiveBoneCount; i++) {
		BoneIDs[i] = 0;
		BoneWeights[i] = 0.0f;
	}
}

AnimatedVertex3D::AnimatedVertex3D() {
	Position = glm::vec3(0.0f);
	Normal = glm::vec3(0.0f);
	TextureCoords = glm::vec2(0.0f);

	InitBoneWeights();
}

AnimatedVertex3D::AnimatedVertex3D(glm::vec3 position, glm::vec3 normal, glm::vec2 textureCoords)
	: Position(position), Normal(normal), TextureCoords(textureCoords) {
	InitBoneWeights();
}

VertexLayout AnimatedVertex3D::GetVertexLayout() {
	VertexLayout layout(sizeof(AnimatedVertex3D));

	layout.Push<float>(3, offsetof(AnimatedVertex3D, Position));
	layout.Push<float>(3, offsetof(AnimatedVertex3D, Normal));
	layout.Push<float>(2, offsetof(AnimatedVertex3D, TextureCoords));
	layout.Push<unsigned int>(4, offsetof(AnimatedVertex3D, BoneIDs));
	layout.Push<float>(4, offsetof(AnimatedVertex3D, BoneWeights));

	return layout;
}