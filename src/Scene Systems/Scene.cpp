#include "../IcePickCoreMinimal.h"
#include "../Render Systems/VertexBuffer.h"
#include "../Render Systems/IndexBuffer.h"
#include "../Render Systems/VertexArray.h"
#include "Scene.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <vector>
#include "../File Systems/FileManager.h"

//static std::vector<unsigned int> indices;
//static std::vector<glm::vec3> vertices;
void IcePick::Scene::OnBegin() {

}

void IcePick::Scene::OnUpdate(DeltaTime dt) {

}

void IcePick::Scene::OnPreRender() {

}

void IcePick::Scene::OnUIRender() {

}

void IcePick::Scene::OnEnd() {

}

void IcePick::Scene::LoadFromDisk(const char* path) {
	IP_LOG("Before loading mesh");
	GLCheckErrors();
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate |
		aiProcess_FixInfacingNormals |
		aiProcess_FlipWindingOrder |
		aiProcess_CalcTangentSpace
	);
	//const aiScene* scene = importer.ReadFile("res/Assets/hatsune_miku.glb", aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::string errorString = "Assimp Error: " + std::string(importer.GetErrorString());
		IP_LOG(errorString.c_str(), IP_ERROR_LOG);
		return;
	}

	std::cout << "Size of vertex: " << sizeof(IcePickRenderer::StaticVertex3D) << std::endl;
	std::cout << "Offset of position: " << offsetof(IcePickRenderer::StaticVertex3D, Position) << std::endl;
	std::cout << "Offset of normal: " << offsetof(IcePickRenderer::StaticVertex3D, Normal) << std::endl;
	std::cout << "Offset of TexCoords: " << offsetof(IcePickRenderer::StaticVertex3D, TextureCoords) << std::endl;

	std::cout << "Hatsune Miku!!!!!!!" << std::endl;
	std::cout << scene->mNumMeshes << std::endl;
	aiMesh** meshList = scene->mMeshes;
	for (int i = 0; i < scene->mNumMeshes; i++)
	//for (int i = 0; i < 1; i++)
	{ // upload each mesh as a vertex array to the GPU
		aiMesh* mesh = meshList[i];
		std::vector<unsigned int> indices;
		std::vector<IcePickRenderer::StaticVertex3D> vertices;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			aiVector3D pos = mesh->mVertices[i];
			aiVector3D uv = aiVector3D(0.0f);
			aiVector3D norm = aiVector3D(0.0f);

			if (mesh->HasTextureCoords(0)) {
				uv = mesh->mTextureCoords[0][i];
			}

			if (mesh->HasNormals()) {
				norm = mesh->mNormals[i];
			}
			glm::vec3 position = glm::vec3(pos.x, pos.y, pos.z);
			glm::vec3 normal = glm::vec3(norm.x, norm.y, norm.z);
			glm::vec2 textureCoords = glm::vec2(uv.x, uv.y);
			vertices.emplace_back(position, normal, textureCoords);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			const aiFace& face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}
		VertexArray& VA = IcePickRenderer::AddVertexArray();
		VA.IndexCount = mesh->mNumFaces * 3;
		VA.Bind();


		VertexBuffer vertexBuffer(vertices.data(), sizeof(IcePickRenderer::StaticVertex3D) * vertices.size());
		vertexBuffer.Bind();
		//VertexBuffer vertexBuffer(mesh->mVertices, sizeof(aiVector3D) * mesh->mNumVertices);

	/*	IcePickRenderer::VertexLayout layout(sizeof(glm::vec3));
		layout.Push<float>(3, offsetof(IcePickRenderer::StaticVertex3D, Position));*/

		
		//IP_LOG("About to add buffer.");
		//GLCheckErrors();
		VA.AddBuffer(vertexBuffer, IcePickRenderer::StaticVertex3D::GetVertexLayout());
		//IP_LOG("Added buffer.");
		//GLCheckErrors();
		//VA.AddBuffer(vertexBuffer, layout);


		IndexBuffer indexBuffer(indices.data(), mesh->mNumFaces * 3);
		indexBuffer.Bind();


		VA.Unbind();
		vertexBuffer.Unbind();
		indexBuffer.Unbind();
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		std::cout << "Mesh " << i << " vertex count: ";
		std::cout << mesh->mNumVertices << std::endl;
		std::cout << "Mesh " << i << " index count: ";
		std::cout << mesh->mNumFaces * 3 << std::endl;
	}
	std::cout << "Done loading scene " << std::endl;
	entt::entity newEntity = IcePick::NewEntity();
}