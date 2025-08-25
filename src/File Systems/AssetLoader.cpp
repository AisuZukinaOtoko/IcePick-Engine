#include "AssetLoader.h"
#include "../IcePickCoreMinimal.h"
#include "../Render Systems/VertexBuffer.h"
#include "../Render Systems/IndexBuffer.h"
#include "../Render Systems/VertexArray.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "../Vendor/glm/gtc/matrix_transform.hpp"

static int debugInt = 0;
static void DebugLogNodeTree(IcePick::MeshNode& node, int depth) {
	std::cout << std::left << std::setw(depth * 3) << "";
	if (node.Children.size() == 0) {
		debugInt++;
		std::cout << "Depth: " << depth << ". ";
		for (auto id : node.VertexArrayIDs) {
			std::cout << id << " ";
		}
		std::cout << std::endl;
		//std::cout << "Mesh: " << node.VertexArrayIDs << ", Depth: " << depth << std::endl;
	}
	else {
		std::cout << "Node" << ", Depth: " << depth << std::endl;
	}

	for (IcePick::MeshNode& meshNode : node.Children) {
		DebugLogNodeTree(meshNode, depth + 1);
	}
}

IcePick::AssetLoader::AssetLoader() {
	for (int i = 0; i < INDEX_COUNT; i++) {
		m_RunningIndices[i] = 0;
	}
}

unsigned int IcePick::AssetLoader::LoadTexture(std::filesystem::path texturePath) {
	UUID textureId = m_TextureLoader.NewTextureFromFile(texturePath);
	return m_TextureLoader.GetTexture(textureId).GetID();
}

const IcePick::Material& IcePick::AssetLoader::GetMaterial(UUID ID) {
	return m_MaterialLoader.GetMaterial(ID, m_TextureLoader);
}

inline unsigned int IcePick::AssetLoader::GetIndex(IndexType indexType, unsigned int index) {
	return index + m_RunningIndices[indexType];
}

void IcePick::AssetLoader::UpdateIndices(const aiScene* scene) {
	m_RunningIndices[MESH_INDEX] += scene->mNumMeshes;
}

IcePick::UUID IcePick::AssetLoader::LoadSceneMaterial(const aiScene* scene, unsigned int materialIndex) {
	return m_MaterialLoader.NewMaterialFromScene(scene, materialIndex, m_TextureLoader);
}

void IcePick::AssetLoader::ProcessSceneNode(const aiNode* sceneNode, MeshNode& parent, const std::vector<unsigned int>& sceneMeshes, const aiScene* scene) {
	MeshNode currentNode;
	if (sceneNode->mNumMeshes > 0) {
		currentNode.VertexArrayIDs.reserve(sceneNode->mNumMeshes);
		for (int i = 0; i < sceneNode->mNumMeshes; i++) {
			const unsigned int meshIndex = sceneNode->mMeshes[i];
			currentNode.VertexArrayIDs.push_back(GetIndex(MESH_INDEX, sceneNode->mMeshes[i]));
			currentNode.MaterialID = LoadSceneMaterial(scene, scene->mMeshes[meshIndex]->mMaterialIndex);
		}
	}

	const aiMatrix4x4& t = sceneNode->mTransformation;
	glm::mat4 nodeTransform = {
		t.a1, t.b1, t.c1, t.d1,
		t.a2, t.b2, t.c2, t.d2,
		t.a3, t.b3, t.c3, t.d3,
		t.a4, t.b4, t.c4, t.d4,
	};

	currentNode.NodeTransform = nodeTransform;
	parent.Children.push_back(currentNode);

	if (sceneNode->mNumChildren == 0)
		return;
	
	for (int i = 0; i < sceneNode->mNumChildren; i++) {
		ProcessSceneNode(sceneNode->mChildren[i], parent.Children.back(), sceneMeshes, scene);
	}
}

void IcePick::AssetLoader::LoadModelMeshData(const aiScene* scene, std::vector<unsigned int>& sceneVertexArrays) {
	aiMesh** meshList = scene->mMeshes;
	for (int i = 0; i < scene->mNumMeshes; i++)	{ // upload each mesh as a vertex array to the GPU

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
		sceneVertexArrays.push_back(VAdata.VA.GetID());
		VertexArray& VA = VAdata.VA;
		VA.IndexCount = mesh->mNumFaces * 3;
		VA.Bind();


		VertexBuffer vertexBuffer(vertices.data(), sizeof(IcePickRenderer::StaticVertex3D) * vertices.size());
		vertexBuffer.Bind();
		VA.AddBuffer(vertexBuffer, IcePickRenderer::StaticVertex3D::GetVertexLayout());


		IndexBuffer indexBuffer(indices.data(), mesh->mNumFaces * 3);
		indexBuffer.Bind();

		MeshComponent newMesh = { VAdata.VA_Index, 1, IcePick::MeshComponent::STATIC };

		VA.Unbind();
		vertexBuffer.Unbind();
		indexBuffer.Unbind();
	}
}

IcePick::MeshRendererComponent IcePick::AssetLoader::LoadMesh(std::filesystem::path filePath)
{
	MeshRendererComponent returnMeshRendererComponent;

	if (m_loadedAssetPathToMeshRenderer.find(filePath) != m_loadedAssetPathToMeshRenderer.end()) {
		return m_loadedAssetPathToMeshRenderer[filePath];
	}

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath.string(), aiProcess_Triangulate |
		aiProcess_FixInfacingNormals |
		aiProcess_CalcTangentSpace
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::string errorString = "Assimp Error: " + std::string(importer.GetErrorString());
		IP_LOG(errorString.c_str(), IP_ERROR_LOG);
		return returnMeshRendererComponent;
	}

	m_TextureLoader.SetLoaderBasePath(filePath.parent_path());
	std::vector<unsigned int> sceneVertexArrays;
	LoadModelMeshData(scene, sceneVertexArrays);
	ProcessSceneNode(scene->mRootNode, returnMeshRendererComponent.RootMeshNode, sceneVertexArrays, scene);
	UpdateIndices(scene);
	CleanUpAfterLoad();

	debugInt = 0;
	returnMeshRendererComponent.MeshCount = scene->mNumMeshes;
	returnMeshRendererComponent.MeshLoaded = true;

	m_loadedAssetPathToMeshRenderer.insert({ filePath, returnMeshRendererComponent });
    return returnMeshRendererComponent;
}

void IcePick::AssetLoader::CleanUpAfterLoad() {
	m_LoadedMaterialIndices.clear();
	m_MaterialLoader.CleanUpAfterLoad();
	m_TextureLoader.CleanUpAfterLoad();
}

void IcePick::AssetLoader::ShutDown() {
	m_MaterialLoader.ShutDown(m_TextureLoader);
	m_TextureLoader.ShutDown();
}
