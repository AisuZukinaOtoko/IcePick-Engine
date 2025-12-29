#pragma once
#include "TextureLoader.h"
#include "ShaderLoader.h"
#include "MaterialAsset.h"
#include "MaterialBase.h"
#include <unordered_map>
#include <string>
#include "../../ASSIMP/include/assimp/scene.h"

namespace IcePick {
	class MaterialLoader {
	public:
		MaterialLoader();
		~MaterialLoader();

		UUID NewMaterialBaseFromCopy(const MaterialBase& newMaterialBase);
		UUID NewMaterialInstanceFromCopy(const MaterialInstance& newMaterialInstance);
		UUID NewMaterialInstanceFromScene(const aiScene* scene, unsigned int materialIndex, TextureLoader& textureLoader);
		UUID NewMaterialBaseFromAsset(std::filesystem::path& assetPath, ShaderLoader& shaderLoader);
		UUID NewMaterialInstanceFromAsset(std::filesystem::path& assetPath);
		void SaveMaterialBase(std::filesystem::path assetPath, const MaterialBase& materialBase);
		void SaveMaterialInstance(std::filesystem::path assetPath, const MaterialInstance& materialInstance);
		
		MaterialBase& GetMaterialBase(UUID Id);
		MaterialInstance& GetMaterialInstance(UUID Id);

		void SetLoadMaterialShaderID(UUID materialShaderId);

		void UpdateMaterialBase(UUID Id, const MaterialBase& newMaterialBase);
		void UpdateMaterialInstance(UUID Id, const MaterialInstance& newMaterialInstance);
		
		void CleanUpAfterLoad();
		void ShutDown(TextureLoader& textureLoader);
	private:
		int m_LoaderVersion = 1;
		enum MaterialTextureTypes {
			DIFFUSE_TEXTURE = 0,
			TYPE_COUNT
		};
		std::vector<std::string> m_DefaultMaterialTextureSamplerIdentifiers;

		UUID m_CachedMaterialBaseId = UUID::Unitialised();
		UUID m_CachedMaterialInstanceId = UUID::Unitialised();

		// Shader ID applied to materials loaded from external files
		UUID m_LoadMaterialShaderId = UUID::Unitialised();

		MaterialBase m_CachedMaterialBase;
		MaterialBase m_DefaultMaterialBase;
		MaterialInstance m_CachedMaterialInstance;
		MaterialInstance m_DefaultMaterialInstance;
		void InvalidateCache();

		// Helper functions when creating a new material
		UUID GetSceneMaterialTexture(const aiScene* scene, aiTextureType textureType, aiMaterial* mat, TextureLoader& textureLoader);
		void SetMaterialInstanceBaseTextureDataFromScene(MaterialInstance& materialInstance, MaterialTextureTypes textureType, const aiScene* scene, unsigned int materialIndex, TextureLoader& textureLoader);

		UUID RegisterMaterialBase(const MaterialBase& materialBase);
		UUID RegisterMaterialInstance(const MaterialInstance& materialInstance);

		std::unordered_map<UUID, MaterialBase, UUIDHasher> m_LoadedMaterialBases;
		std::unordered_map<UUID, MaterialInstance, UUIDHasher> m_LoadedMaterialInstances;
		
		std::unordered_map<std::filesystem::path, UUID> m_CachedMaterialBaseAssetPaths;
		std::unordered_map<std::filesystem::path, UUID> m_CachedMaterialInstanceAssetPaths;

		// Cache for loaded scene material instances used during loading a multiple materials within a scene. Cleared after each scene is loaded.
		std::unordered_map<unsigned int, UUID> m_CachedSceneMaterialInstances;
	};
}

