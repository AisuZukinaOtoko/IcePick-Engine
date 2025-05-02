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
	unsigned int meshID = GlobalFileSystem.LoadMesh<FBX>("res/Assets/cube.fbx");
	std::cout << "Loaded mesh" << std::endl;
	Mesh& tempMesh = GlobalFileSystem.GetMesh(meshID);
	//IcePickRenderer::AddGeometry(tempMesh);


	VertexArray& VA = IcePickRenderer::AddVertexArray();
	VA.IndexCount = tempMesh.m_Indices.size();
	VA.Bind();


	VertexBuffer vertexBuffer(tempMesh.m_Vertices.data(), sizeof(IcePick::Vertex) * tempMesh.m_Vertices.size());
	vertexBuffer.Bind();
	//VertexBuffer vertexBuffer(mesh->mVertices, sizeof(aiVector3D) * mesh->mNumVertices);

	VertexBufferLayout layout;
	layout.Push<float>(3);
	layout.Push<float>(2);
	layout.Push<float>(3);

	VA.AddBuffer(vertexBuffer, layout);


	IndexBuffer indexBuffer(tempMesh.m_Indices.data(), tempMesh.m_Indices.size());
	indexBuffer.Bind();


	VA.Unbind();
	vertexBuffer.Unbind();
	indexBuffer.Unbind();
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	return;
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate);
	//const aiScene* scene = importer.ReadFile("res/Assets/hatsune_miku.glb", aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "Assimp Error: " << importer.GetErrorString() << std::endl;
		return;
	}

	std::cout << "Hatsune Miku!!!!!!!" << std::endl;
	std::cout << scene->mNumMeshes << std::endl;
	aiMesh** meshList = scene->mMeshes;
	//for (int i = 0; i < scene->mNumMeshes; i++)
	for (int i = 0; i < 1; i++)
	{ // upload each mesh as a vertex array to the GPU
		aiMesh* mesh = meshList[i];
		std::vector<unsigned int> indices;
		std::vector<glm::vec3> vertices;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			aiVector3D pos = mesh->mVertices[i];
			vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
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


		VertexBuffer vertexBuffer(vertices.data(), sizeof(glm::vec3) * vertices.size());
		vertexBuffer.Bind();
		//VertexBuffer vertexBuffer(mesh->mVertices, sizeof(aiVector3D) * mesh->mNumVertices);

		VertexBufferLayout layout;
		layout.Push<float>(3);

		VA.AddBuffer(vertexBuffer, layout);


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
	entt::entity newEntity = IcePick::NewEntity();
}