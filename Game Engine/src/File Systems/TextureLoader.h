#pragma once
#include "../Render Systems/Texture.h"
#include "../Scene Systems/UUID.h"
#include <filesystem>
#include <unordered_map>
#include "assimp/scene.h"

namespace IcePick {
	class TextureLoader {
	public:
		TextureLoader();
		void ShutDown();
		~TextureLoader();

		UUID NewTextureFromFile(std::filesystem::path texturePath);
		UUID NewTextureFromMemory(unsigned char* data);
		UUID NewTextureFromScene(std::string texturePath, const aiScene* scene);
		UUID NewTextureFromAsset(std::filesystem::path& assetPath);
		const Texture& GetTexture(UUID id);
		const Texture& GetDefaultTexture();
		const UUID GetDefaultTextureID();
		void SetLoaderBasePath(std::filesystem::path filePath);
		void UpdateTexture(UUID id, const Texture& other);
		void CleanUpAfterLoad();
	private:
		bool NewTextureFromFileWithID(std::filesystem::path texturePath, UUID textureId);

		UUID m_CachedTextureId = UUID::Unitialised();
		UUID m_DefaultTextureId = UUID::Unitialised();
		Texture m_CachedTexture;
		const char* m_DefaultTextureRelativePath = "Game Engine/res/Textures/DefaultTexture.png";
		std::filesystem::path m_DefaultTexturePath;
		std::filesystem::path m_BaseFilePath;
		Texture m_DefaultTexture;
		UUID RegisterTexture(const Texture& texture);
		std::unordered_map<UUID, Texture, UUIDHasher> m_LoadedTextures;
		std::unordered_map<std::filesystem::path, UUID> m_CachedTexturePaths;
		std::unordered_map<std::filesystem::path, UUID> m_CachedTextureAssetPaths;


		// Cache for loaded scene textures used during loading multiple materials within a scene. Cleared after each scene is loaded.
		// Loaded materials within a scene contain indexed texture paths such as '*1'. This cache is to avoid loading duplicate indexed texture paths.
		std::unordered_map<std::string, UUID> m_CachedSceneTexturePaths;
	};
}