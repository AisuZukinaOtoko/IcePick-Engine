#include "TextureLoader.h"
#include "../LogSystem.h"

namespace IcePick {
	TextureLoader::TextureLoader()
	: m_DefaultTexture(m_DefaultTexturePath.string()),
	m_CachedTexture(m_DefaultTexture) {
		m_CachedTexturePaths.insert({ m_DefaultTexturePath, UUID::Unitialised() });
	}

	UUID TextureLoader::RegisterTexture(const Texture& texture)	{
		UUID textureId;
		m_LoadedTextures.insert({ textureId, texture });
		return textureId;
	}

	UUID TextureLoader::NewTextureFromFile(std::filesystem::path texturePath) {
		texturePath = std::filesystem::canonical(m_BaseFilePath / texturePath); // resolve symlinks and relative paths.
		auto iterator = m_CachedTexturePaths.find(texturePath);
		
		// Texture has already been loaded.
		if (iterator != m_CachedTexturePaths.end()) {
			return iterator->second;
		}

		Texture newTexture(texturePath.string());
		UUID newTextureId = RegisterTexture(newTexture);
		m_CachedTexturePaths.insert({ texturePath, newTextureId });
		return newTextureId;
	}

	UUID TextureLoader::NewTextureFromMemory(unsigned char* data) {
		if (!data) {
			IP_LOG("Texture has null data.", IP_ERROR_LOG);
			return UUID::Unitialised();
		}

		IP_LOG("Loading a texture from memory not implemented.", IP_WARN_LOG);
		return UUID::Unitialised();
	}

	UUID TextureLoader::NewTextureFromScene(std::string texturePath, const aiScene* scene) {
		// Check if scene texture has been loaded.
		auto iterator = m_CachedSceneTexturePaths.find(texturePath);
		if (iterator != m_CachedSceneTexturePaths.end())
			return iterator->second;

		if (texturePath[0] != '*') { // External texture file
			return NewTextureFromFile(texturePath);
		}

		int texIndex = std::atoi(texturePath.c_str() + 1);
		IP_ASSERT((texIndex >= 0) && (texIndex < scene->mNumTextures), "Invalid scene texture index.");

		const aiTexture* tex = scene->mTextures[texIndex];
		UUID newTextureId;
		if (tex->mHeight == 0) { // Compressed embedded texture
			Texture newTexture(reinterpret_cast<unsigned char*>(tex->pcData), tex->mWidth);
			newTextureId = RegisterTexture(newTexture);
		}
		else { // Raw uncompressed embedded texture
			const int numTextureChannels = 4;
			Texture newTexture(reinterpret_cast<unsigned char*>(tex->pcData), tex->mWidth, tex->mHeight, numTextureChannels);
			newTextureId = RegisterTexture(newTexture);
		}

		m_CachedSceneTexturePaths.insert({ texturePath, newTextureId });
		return newTextureId;
	}

	const Texture& TextureLoader::GetTexture(UUID id) {
		if (id == UUID::Unitialised())
			return m_DefaultTexture;

		if (id == m_CachedTextureId)
			return m_CachedTexture;

		m_CachedTextureId = id;
		auto iterator = m_LoadedTextures.find(id);
		if (iterator == m_LoadedTextures.end()) {
			m_CachedTexture = m_CachedTexture;
			return m_DefaultTexture;
		}

		m_CachedTexture = iterator->second;
		return iterator->second;
	}

	const Texture& TextureLoader::GetDefaultTexture() {
		return m_DefaultTexture;
	}

	void TextureLoader::SetLoaderBasePath(std::filesystem::path filePath) {
		m_BaseFilePath = filePath;
	}

	void TextureLoader::UpdateTexture(UUID id, const Texture& other) {

	}	

	void TextureLoader::ShutDown() {
		for (auto iterator = m_LoadedTextures.begin(); iterator != m_LoadedTextures.end(); ++iterator) {
			iterator->second.Destroy();
		}
		m_LoadedTextures.clear();
		m_CachedTexturePaths.clear();
	}

	void TextureLoader::CleanUpAfterLoad() {
		m_CachedSceneTexturePaths.clear();
		m_BaseFilePath.clear();
	}

	TextureLoader::~TextureLoader() {
		ShutDown();
	}
}