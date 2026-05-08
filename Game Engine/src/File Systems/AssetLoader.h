#pragma once
#include "TextureLoader.h"
#include "MaterialLoader.h"
#include "ShaderLoader.h"
#include "../Scene Systems/Components.h"
#include <filesystem>
#include <unordered_map>
#include <unordered_set>
#include "assimp/scene.h"

namespace IcePick {

	class AssetLoader {
	public:
		AssetLoader();
		void Init();
		MeshRendererComponent LoadMesh(std::filesystem::path filePath);
		unsigned int LoadTexture(std::filesystem::path texturePath);
		unsigned int GetTextureRenderId(UUID textureId);
		UUID LoadTextureFromAsset(std::filesystem::path& assetPath);
		UUID CreateShaderFromSource(ShaderSource& source);

		MaterialBase& GetMaterialBase(UUID Id);
		MaterialInstance& GetMaterialInstance(UUID Id);

		const Texture& GetTexture(UUID Id);
		ShaderProgram& GetShaderProgram(UUID Id);
		ShaderProgram& GetDefaultShaderProgram(ShaderLoader::DefaultShader shaderType);
		void ReloadShaderPrograms();
		void ShutDown();
	private:
		friend class EngineAPI;
		UUID m_PBRShaderProgramId = UUID::Unitialised();
		const char* m_PBRVertShader = "Game Engine/res/shaders/pbr.vert.shader";
		const char* m_PBRFragShader = "Game Engine/res/shaders/pbr.frag.shader";

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
		std::unordered_set<unsigned int> m_LoadedMaterialIndices;

		unsigned int m_RunningIndices[INDEX_COUNT];

		void ProcessSceneNode(const aiNode* sceneNode, MeshNode& parent, std::vector<UUID>& materialSlots, const aiScene* scene);
		void LoadModelMeshData(const aiScene* scene, std::vector<unsigned int>& sceneVertexArrays);
	};
}