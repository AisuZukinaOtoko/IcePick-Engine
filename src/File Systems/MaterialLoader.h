#pragma once
#include "TextureLoader.h"
#include "MaterialAsset.h"
#include <unordered_map>
#include <string>
#include "../Render Systems/Materials.h"
#include "../../ASSIMP/include/assimp/scene.h"

namespace IcePick {
	class MaterialLoader {
	public:
		MaterialLoader();
		~MaterialLoader();

		UUID NewMaterialFromScene(const aiScene* scene, unsigned int materialIndex, TextureLoader& textureLoader);
		Material GetMaterial(UUID id, TextureLoader& textureLoader);
		void UpdateMaterial(const Material& other);
		void CleanUpAfterLoad();
		void ShutDown(TextureLoader& textureLoader);
	private:
		UUID m_CachedMaterialId = UUID::Unitialised();
		Material m_CachedMaterial;
		Material m_DefaultMaterial;
		void ConstructMaterial(const MaterialAsset& mat, Material& result, TextureLoader& textureLoader) const;

		// Helper functions when creating a new material
		UUID GetSceneMaterialTexture(const aiScene* scene, aiTextureType textureType, aiMaterial* mat, TextureLoader& textureLoader);
		void GetSceneMaterialColours(MaterialAsset& materialAsset, aiMaterial* mat);
		UUID RegisterMaterialAsset(const MaterialAsset& materialAsset);
		void SetMaterialSampleBits(MaterialAsset& materialAsset, UUID defualtTextureId);

		std::unordered_map<UUID, MaterialAsset, UUIDHasher> m_LoadedMaterialAssets;
		
		// Cache for loaded scene materials used during loading a multiple materials within a scene. Cleared after each scene is loaded.
		std::unordered_map<unsigned int, UUID> m_CachedSceneMaterials;
	};
}

