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
	struct Asset {
		unsigned int ID = -1; // index
		bool AssetLoaded = false;
		std::filesystem::path FilePath;
	};

	class AssetLoader {
	public:
		AssetLoader();
		MeshRendererComponent LoadMesh(std::filesystem::path filePath);
		unsigned int LoadTexture(std::filesystem::path texturePath);
		const Material& GetMaterial(UUID ID);
		void ShutDown();
	private:
		enum IndexType {
			MESH_INDEX = 0,
			INDEX_COUNT
		};
		inline unsigned int GetIndex(IndexType indexType, unsigned int index);
		void UpdateIndices(const aiScene* scene);
		UUID LoadSceneMaterial(const aiScene* scene, unsigned int materialIndex);
		void CleanUpAfterLoad();
		std::unordered_map<std::filesystem::path, MeshRendererComponent> m_loadedAssetPathToMeshRenderer;

		TextureLoader m_TextureLoader;
		MaterialLoader m_MaterialLoader;
		ShaderLoader m_ShaderLoader;
		std::unordered_set<unsigned int> m_LoadedMaterialIndices;

		unsigned int m_RunningIndices[INDEX_COUNT];

		void ProcessSceneNode(const aiNode* sceneNode, MeshNode& parent, const std::vector<unsigned int>& sceneMeshes, const aiScene* scene);
		void LoadModelMeshData(const aiScene* scene, std::vector<unsigned int>& sceneVertexArrays);
	};
}