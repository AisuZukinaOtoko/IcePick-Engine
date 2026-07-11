#include "AssetLoader.h"
#include "../LogSystem.h"
#include "../Scene Systems/Components.h"
#include "../Render Systems/Vertex.h"
#include "../Render Systems/VertexBuffer.h"
#include "../Render Systems/VertexArray.h"
#include "../Render Systems/VertexLayout.h"
#include "../Render Systems/IndexBuffer.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>

IcePick::AssetLoader::AssetLoader() {
	for (int i = 0; i < INDEX_COUNT; i++) {
		m_RunningIndices[i] = 0;
	}
}

void IcePick::AssetLoader::Init() {
	m_ShaderLoader.Init();
	m_MaterialLoader.Init(m_ShaderLoader);
	m_MeshLoader.Init(m_ShaderLoader);
}

unsigned int IcePick::AssetLoader::LoadTexture(std::filesystem::path texturePath) {
	UUID textureId = m_TextureLoader.NewTextureFromFile(texturePath);
	return m_TextureLoader.GetTexture(textureId).GetID();
}

unsigned int IcePick::AssetLoader::GetTextureRenderId(UUID textureId) {
	return m_TextureLoader.GetTexture(textureId).GetID();
}

IcePick::UUID IcePick::AssetLoader::LoadTextureFromAsset(std::filesystem::path& assetPath) {
	return m_TextureLoader.NewTextureFromAsset(assetPath);
}

IcePick::UUID IcePick::AssetLoader::CreateShaderFromSource(ShaderSource& source) {
	return m_ShaderLoader.CreateShaderProgram(source);
}

IcePick::MaterialBase& IcePick::AssetLoader::GetMaterialBase(IcePick::UUID Id) {
	return m_MaterialLoader.GetMaterialBase(Id);
}

IcePick::MaterialInstance& IcePick::AssetLoader::GetMaterialInstance(IcePick::UUID Id) {
	return m_MaterialLoader.GetMaterialInstance(Id);
}

const IcePickRenderer::Texture& IcePick::AssetLoader::GetTexture(UUID Id) {
	return m_TextureLoader.GetTexture(Id);
}

std::string IcePick::AssetLoader::LoadShaderSourceFromFile(std::filesystem::path filepath) {
	return m_ShaderLoader.LoadFile(filepath, 0);
}

IcePick::ShaderProgram& IcePick::AssetLoader::GetShaderProgram(UUID Id) {
	return m_ShaderLoader.GetShaderProgram(Id);;
}

IcePick::ShaderProgram& IcePick::AssetLoader::GetDefaultShaderProgram(ShaderLoader::DefaultShader shaderType) {
	return m_ShaderLoader.GetDefaultShaderProgram(shaderType);
}

void IcePick::AssetLoader::ReloadShaderPrograms() {
	IP_LOG("Reloading shaders has been disabled.", IP_WARN_LOG);
}

inline unsigned int IcePick::AssetLoader::GetIndex(IndexType indexType, unsigned int index) {
	return index + m_RunningIndices[indexType];
}

void IcePick::AssetLoader::UpdateIndices(const aiScene* scene) {
	m_RunningIndices[MESH_INDEX] += scene->mNumMeshes;
}

IcePick::UUID IcePick::AssetLoader::LoadSceneMaterial(const aiScene* scene, unsigned int materialIndex) {
	return m_MaterialLoader.NewMaterialInstanceFromScene(scene, materialIndex, m_TextureLoader, ImportSettings{});
}

unsigned int IcePick::AssetLoader::GetMeshMaterialSlot(std::vector<UUID>& materialSlots, UUID meshMaterial) {
	//for (unsigned int i = 0; i < materialSlots.size(); i++) {
	//	if (materialSlots[i] == meshMaterial)
	//		return i;
	//}

	//materialSlots.push_back(meshMaterial);
	//return (unsigned int)materialSlots.size() - 1;
	return 0;
}

void IcePick::AssetLoader::ProcessSceneNode(const aiNode* sceneNode, MeshNode& parent, std::vector<UUID>& materialSlots, const aiScene* scene) {
	//MeshNode currentNode;
	//if (sceneNode->mNumMeshes > 0) {
	//	currentNode.VertexArrayIDs.reserve(sceneNode->mNumMeshes);
	//	for (int i = 0; i < sceneNode->mNumMeshes; i++) {
	//		const unsigned int meshIndex = sceneNode->mMeshes[i];
	//		currentNode.VertexArrayIDs.push_back(GetIndex(MESH_INDEX, sceneNode->mMeshes[i]));
	//		UUID nodeMaterialInsatnceId = LoadSceneMaterial(scene, scene->mMeshes[meshIndex]->mMaterialIndex);
	//		unsigned int materialSlotIndex = GetMeshMaterialSlot(materialSlots, nodeMaterialInsatnceId);
	//		currentNode.MaterialSlotIndices.push_back(materialSlotIndex);
	//	}
	//}

	//const aiMatrix4x4& t = sceneNode->mTransformation;
	//glm::mat4 nodeTransform = {
	//	t.a1, t.b1, t.c1, t.d1,
	//	t.a2, t.b2, t.c2, t.d2,
	//	t.a3, t.b3, t.c3, t.d3,
	//	t.a4, t.b4, t.c4, t.d4,
	//};

	//currentNode.NodeTransform = nodeTransform;
	//parent.Children.push_back(currentNode);

	//if (sceneNode->mNumChildren == 0)
	//	return;
	//
	//for (int i = 0; i < sceneNode->mNumChildren; i++) {
	//	ProcessSceneNode(sceneNode->mChildren[i], parent.Children.back(), materialSlots, scene);
	//}
}

void IcePick::AssetLoader::LoadModelMeshData(const aiScene* scene, std::vector<unsigned int>& sceneVertexArrays) {
	//aiMesh** meshList = scene->mMeshes;
	//for (int i = 0; i < scene->mNumMeshes; i++)	{ // upload each mesh as a vertex array to the GPU

	//	aiMesh* mesh = meshList[i];
	//	std::vector<unsigned int> indices;
	//	std::vector<IcePickRenderer::StaticVertex3D> vertices;

	//	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
	//		aiVector3D pos = mesh->mVertices[i];
	//		aiVector3D uv = aiVector3D(0.0f);
	//		aiVector3D norm = aiVector3D(0.0f);

	//		if (mesh->HasTextureCoords(0)) {
	//			uv = mesh->mTextureCoords[0][i];
	//		}

	//		if (mesh->HasNormals()) {
	//			norm = mesh->mNormals[i];
	//		}

	//		glm::vec3 position = glm::vec3(pos.x, pos.y, pos.z);
	//		glm::vec3 normal = glm::vec3(norm.x, norm.y, norm.z);
	//		glm::vec2 textureCoords = glm::vec2(uv.x, uv.y);
	//		vertices.emplace_back(position, normal, textureCoords);
	//	}

	//	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
	//		const aiFace& face = mesh->mFaces[i];
	//		for (unsigned int j = 0; j < face.mNumIndices; j++) {
	//			indices.push_back(face.mIndices[j]);
	//		}
	//	}

	//	if (mesh->HasBones()) { // Animated skeletal Mesh
	//		IP_LOG("Mesh has bones.");
	//	}

	//	NewVertexArrayData VAdata = IcePickRenderer::AddVertexArray();
	//	VertexArray& VA = VAdata.VA;
	//	VA.IndexCount = mesh->mNumFaces * 3;
	//	VA.Bind();


	//	VertexBuffer vertexBuffer(vertices.data(), sizeof(IcePickRenderer::StaticVertex3D) * vertices.size());
	//	vertexBuffer.Bind();
	//	VA.AddBuffer(vertexBuffer, IcePickRenderer::StaticVertex3D::GetVertexLayout());


	//	IndexBuffer indexBuffer(indices.data(), mesh->mNumFaces * 3);
	//	indexBuffer.Bind();

	//	MeshComponent newMesh = { VAdata.VA_Index, 1, IcePick::MeshComponent::STATIC };

	//	VA.Unbind();
	//	vertexBuffer.Unbind();
	//	indexBuffer.Unbind();
	//}
}

IcePick::MeshRendererComponent IcePick::AssetLoader::LoadMesh(std::filesystem::path filePath, IcePick::ImportSettings importSettings) {
	return m_MeshLoader.ImportMesh(filePath, m_MaterialLoader, m_TextureLoader, importSettings);
	//MeshRendererComponent returnMeshRendererComponent;

	//if (m_loadedAssetPathToMeshRenderer.find(filePath) != m_loadedAssetPathToMeshRenderer.end()) {
	//	return m_loadedAssetPathToMeshRenderer[filePath];
	//}

	//Assimp::Importer importer;
	//const aiScene* scene = importer.ReadFile(filePath.string(), aiProcess_Triangulate |
	//	aiProcess_FixInfacingNormals |
	//	aiProcess_CalcTangentSpace |
	//	aiProcess_GenUVCoords |
	//	aiProcess_OptimizeMeshes
	//);

	//if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
	//	std::string errorString = "Assimp Error: " + std::string(importer.GetErrorString());
	//	IP_LOG(errorString.c_str(), IP_ERROR_LOG);
	//	return returnMeshRendererComponent;
	//}

	//m_TextureLoader.SetLoaderBasePath(filePath.parent_path());
	//m_MaterialLoader.SetLoadMaterialShaderID(m_PBRShaderProgramId);
	//std::vector<unsigned int> sceneVertexArrays;
	//LoadModelMeshData(scene, sceneVertexArrays);
	//ProcessSceneNode(scene->mRootNode, returnMeshRendererComponent.RootMeshNode, returnMeshRendererComponent.MaterialSlots, scene);
	//UpdateIndices(scene);
	//CleanUpAfterLoad();

	//returnMeshRendererComponent.MeshCount = scene->mNumMeshes;
	//returnMeshRendererComponent.MeshLoaded = true;

	//m_loadedAssetPathToMeshRenderer.insert({ filePath, returnMeshRendererComponent });
    //return returnMeshRendererComponent;
}

IcePickRenderer::MeshNode& IcePick::AssetLoader::GetMeshData(const MeshRendererComponent& meshRendererComponent) {
	if (meshRendererComponent.MeshType == IcePick::ImportSettings::MeshType::SKELETAL_MESH) {
		IcePickRenderer::SkinnedMeshData& skinnedMeshData = m_MeshLoader.GetSkinnedMeshById(meshRendererComponent.meshDataId);
		return skinnedMeshData.RootNode;
	}
	else {
		IcePickRenderer::StaticMeshData& staticMeshData = m_MeshLoader.GetStaticMeshById(meshRendererComponent.meshDataId);
		return staticMeshData.RootNode;
	}
}

IcePickRenderer::VertexArray& IcePick::AssetLoader::GetMeshVertexArray(IcePick::UUID vertexArrayId) {
	return m_MeshLoader.GetMeshNodeVertexArray(vertexArrayId);
}

IcePickRenderer::StaticMeshData& IcePick::AssetLoader::GetStaticMeshData(const MeshRendererComponent& meshRendererComponent) {
	return m_MeshLoader.GetStaticMeshById(meshRendererComponent.meshDataId);
}

IcePickRenderer::SkinnedMeshData& IcePick::AssetLoader::GetSkinnedMeshData(const MeshRendererComponent& meshRendererComponent) {
	return m_MeshLoader.GetSkinnedMeshById(meshRendererComponent.meshDataId);
}

IcePick::Skeleton& IcePick::AssetLoader::GetSkeletonById(IcePick::UUID skeletonId) {
	return m_MeshLoader.GetSkeletonById(skeletonId);
}

void IcePick::AssetLoader::CleanUpAfterLoad() {
	m_LoadedMaterialIndices.clear();
	m_MaterialLoader.CleanUpAfterLoad();
	m_TextureLoader.CleanUpAfterLoad();
	m_MeshLoader.Destroy();
}

void IcePick::AssetLoader::ShutDown() {
	m_MaterialLoader.ShutDown(m_TextureLoader);
	m_TextureLoader.ShutDown();
}
