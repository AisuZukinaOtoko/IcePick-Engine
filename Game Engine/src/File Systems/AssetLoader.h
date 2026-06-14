#pragma once
#include "TextureLoader.h"
#include "MaterialLoader.h"
#include "ShaderLoader.h"
#include "MeshLoader.h"
#include <filesystem>
#include <unordered_map>
#include <unordered_set>
#include <assimp/scene.h>

namespace IcePick {
	struct MeshRendererComponent;
	struct MeshNode;

	class AssetLoader {
	public:
		AssetLoader();
		void Init();
		MeshRendererComponent LoadMesh(std::filesystem::path filePath, IcePick::ImportSettings importSettings);
		IcePickRenderer::MeshNode& GetMeshData(const MeshRendererComponent& meshRenderer);
		IcePickRenderer::VertexArray& GetMeshVertexArray(UUID vertexArrayId);

		unsigned int LoadTexture(std::filesystem::path texturePath);
		UUID LoadTextureFromAsset(std::filesystem::path& assetPath);
		unsigned int GetTextureRenderId(UUID textureId);
		const IcePickRenderer::Texture& GetTexture(UUID Id);

		MaterialBase& GetMaterialBase(UUID Id);
		MaterialInstance& GetMaterialInstance(UUID Id);


		UUID CreateShaderFromSource(ShaderSource& source);
		std::string LoadShaderSourceFromFile(std::filesystem::path filepath);
		ShaderProgram& GetShaderProgram(UUID Id);
		ShaderProgram& GetDefaultShaderProgram(ShaderLoader::DefaultShader shaderType);
		void ReloadShaderPrograms();

		void ShutDown();
	private:
		friend class EngineAPI;
		UUID m_PBRShaderProgramId = UUID::Unitialised();

		enum IndexType {
			MESH_INDEX = 0,
			INDEX_COUNT
		};
		inline unsigned int GetIndex(IndexType indexType, unsigned int index);
		void UpdateIndices(const aiScene* scene);
		UUID LoadSceneMaterial(const aiScene* scene, unsigned int materialIndex);
		unsigned int GetMeshMaterialSlot(std::vector<UUID>& materialSlots, UUID meshMaterial);
		void CleanUpAfterLoad();
		std::unordered_map<std::filesystem::path, MeshRendererComponent> m_loadedAssetPathToMeshRenderer;

		TextureLoader m_TextureLoader;
		MaterialLoader m_MaterialLoader;
		ShaderLoader m_ShaderLoader;
		MeshLoader m_MeshLoader;
		std::unordered_set<unsigned int> m_LoadedMaterialIndices;

		unsigned int m_RunningIndices[INDEX_COUNT];

		void ProcessSceneNode(const aiNode* sceneNode, MeshNode& parent, std::vector<UUID>& materialSlots, const aiScene* scene);
		void LoadModelMeshData(const aiScene* scene, std::vector<unsigned int>& sceneVertexArrays);
	};
}