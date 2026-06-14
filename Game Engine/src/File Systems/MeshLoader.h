#pragma once
#include "../Render Systems/Mesh.h"
#include "../Render Systems/VertexArray.h"
#include "../Scene Systems/UUID.h"
#include "../Scene Systems/Components.h"
#include "ImportSettings.h"
#include <filesystem>
#include <unordered_map>

namespace IcePick {
	class MaterialLoader;
	class TextureLoader;

	class MeshLoader {
	public:
		MeshLoader();
		void Init();
		MeshRendererComponent ImportMesh(std::filesystem::path filePath, MaterialLoader& materialLoader, TextureLoader& textureLoader, const ImportSettings& importSettings);
		MeshRendererComponent LoadMeshAsset(std::filesystem::path assetPath);

		IcePickRenderer::StaticMeshData& GetStaticMeshById(UUID staticMeshId);
		IcePickRenderer::SkinnedMeshData& GetSkinnedMeshById(UUID skinnedMeshId);

		IcePickRenderer::VertexArray& GetMeshNodeVertexArray(UUID vertexArrayId);
		
		void Destroy();
		~MeshLoader();
	private:
		IcePickRenderer::VertexArray m_DefaultInvalidVertexArray;
		IcePickRenderer::StaticMeshData m_DefaultEmptyStaticMesh;
		IcePickRenderer::SkinnedMeshData m_DefaultEmptySkinnedMesh;

		UUID RegisterVertexArray(const IcePickRenderer::VertexArray& vertexArray);
		UUID RegisterStaticMesh(const IcePickRenderer::StaticMeshData staticMesh);
		UUID RegisterSkinnedMesh(const IcePickRenderer::SkinnedMeshData skinnedMesh);

		unsigned int GetMeshMaterialSlot(std::vector<UUID>& materialSlots, UUID meshMaterial);
		void ParseImportMeshData(const aiScene* scene, std::vector<UUID>& loadVertexArrays, const ImportSettings& importSettings);
		void ParseImportNodeTree(const aiNode* sceneNode, IcePickRenderer::MeshNode& parent, std::vector<UUID>& materialSlots, const aiScene* scene, std::vector<UUID>& sceneVertexArrayIds, MaterialLoader& materialLoader, TextureLoader& textureLoader);

		std::unordered_map<UUID, IcePickRenderer::StaticMeshData, UUIDHasher> m_LoadedStaticMeshes;
		std::unordered_map<UUID, IcePickRenderer::SkinnedMeshData, UUIDHasher> m_LoadedSkinnedMeshes;

		std::unordered_map<std::filesystem::path, MeshRendererComponent> m_LoadedPathToMeshRenderer;

		std::unordered_map<UUID, IcePickRenderer::VertexArray, UUIDHasher> m_LoadedVertexArrays;
	};


}