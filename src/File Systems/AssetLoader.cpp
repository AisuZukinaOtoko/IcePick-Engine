#include "AssetLoader.h"
#include "../IcePickCoreMinimal.h"
#include "../Render Systems/VertexBuffer.h"
#include "../Render Systems/IndexBuffer.h"
#include "../Render Systems/VertexArray.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

IcePick::AssetLoader::AssetLoader() {
	m_loadedTextures.reserve(5);
	m_loadedTextures.emplace_back("res/textures/icons/folder_icon.png");
	m_loadedTextures.emplace_back("res/textures/icons/fbx_icon.png");
	m_loadedTextures.emplace_back("res/textures/icons/glb_icon.png");
	m_loadedTextures.emplace_back("res/textures/icons/obj_icon.png");
	m_loadedTextures.emplace_back("res/textures/icons/file_icon.png");
	std::cout << "Texture ID: " << m_loadedTextures.back().GetID() << std::endl;
}

IcePick::MeshRendererComponent IcePick::AssetLoader::LoadMesh(std::filesystem::path filePath)
{
	MeshRendererComponent returnMeshRendererComponent;

	if (m_loadedAssetPathToMeshRenderer.find(filePath) != m_loadedAssetPathToMeshRenderer.end()) {
		IP_LOG("Using a loaded mesh.");
		const MeshRendererComponent& savedMeshRenderer = m_loadedAssetPathToMeshRenderer[filePath];
		returnMeshRendererComponent.MeshCount = savedMeshRenderer.MeshCount;
		returnMeshRendererComponent.MeshLoaded = savedMeshRenderer.MeshLoaded;
		returnMeshRendererComponent.MeshFilePath = savedMeshRenderer.MeshFilePath;

		for (int i = 0; i < savedMeshRenderer.MeshCount; i++) {
			returnMeshRendererComponent.Meshes[i] = savedMeshRenderer.Meshes[i];
		}
		
		return returnMeshRendererComponent;
	}

	IP_LOG("Loading a new mesh.");
	//GLCheckErrors();
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath.string(), aiProcess_Triangulate |
		aiProcess_FixInfacingNormals |
		aiProcess_FlipWindingOrder |
		aiProcess_CalcTangentSpace
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::string errorString = "Assimp Error: " + std::string(importer.GetErrorString());
		IP_LOG(errorString.c_str(), IP_ERROR_LOG);
		return returnMeshRendererComponent;
	}	

	aiMesh** meshList = scene->mMeshes;
	for (int i = 0; i < scene->mNumMeshes; i++)
	{ // upload each mesh as a vertex array to the GPU
		if (i >= MeshRendererComponent::MaxMeshCount) {
			IP_LOG("Exceeded the max number of meshes for the mesh renderer.", IP_WARN_LOG);
			break;
		}

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
		NewVertexArrayData VAdata = IcePickRenderer::AddVertexArray();
		VertexArray& VA = VAdata.VA;
		VA.IndexCount = mesh->mNumFaces * 3;
		VA.Bind();


		VertexBuffer vertexBuffer(vertices.data(), sizeof(IcePickRenderer::StaticVertex3D) * vertices.size());
		vertexBuffer.Bind();
		VA.AddBuffer(vertexBuffer, IcePickRenderer::StaticVertex3D::GetVertexLayout());


		IndexBuffer indexBuffer(indices.data(), mesh->mNumFaces * 3);
		indexBuffer.Bind();

		returnMeshRendererComponent.Meshes[i] = { VAdata.VA_Index, 1, IcePick::MeshComponent::STATIC};
		//returnMeshRendererComponent.Meshes[i] = { VAdata.VA_Index, -1, IcePick::MeshComponent::STATIC};

		VA.Unbind();
		vertexBuffer.Unbind();
		indexBuffer.Unbind();
	}
	returnMeshRendererComponent.MeshCount = scene->mNumMeshes;
	returnMeshRendererComponent.MeshLoaded = true;

	m_loadedAssetPathToMeshRenderer.insert({ filePath, returnMeshRendererComponent });
    return returnMeshRendererComponent;
}
