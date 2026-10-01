#include "TextureLoader.h"
#include <nlohmann/json.hpp>
#include "../Utilities/JsonUtils.h"
#include "../LogSystem.h"
#include "../Utilities/Assert.h"
#include <fstream>

namespace IcePick {
	TextureLoader::TextureLoader()
	: m_DefaultTexture(m_DefaultTextureRelativePath, true),
	m_CachedTexture(m_DefaultTexture) {
		m_DefaultTexturePath = std::filesystem::canonical(m_DefaultTextureRelativePath);
		m_DefaultTextureId = UUID();
	}

	UUID TextureLoader::RegisterTexture(const IcePickRenderer::Texture& texture)	{
		if (!texture.IsValid()) {
			return m_DefaultTextureId;
		}

		UUID textureId;
		m_LoadedTextures.insert({ textureId, texture });
		return textureId;
	}

	void TextureLoader::InvalidateCache() {
		m_CachedTextureId = UUID::Unitialised();
	}

	UUID TextureLoader::NewTextureFromFile(std::filesystem::path loadTexturePath, bool isNonLinearSpace) {
		std::error_code errorCode;
		std::filesystem::path texturePath = std::filesystem::canonical(m_BaseFilePath / loadTexturePath, errorCode); // resolve symlinks and relative paths.
		if (errorCode) {
			IP_LOG("Error resolving path: " + loadTexturePath.string() + ". " + errorCode.message(), IP_ERROR_LOG);
			return UUID::Unitialised();
		}

		if (texturePath == m_DefaultTexturePath)
			return m_DefaultTextureId;

		auto iterator = m_CachedTexturePaths.find(texturePath);
		// Texture has already been loaded.
		if (iterator != m_CachedTexturePaths.end()) {
			return iterator->second;
		}

		IcePickRenderer::Texture newTexture(texturePath.string(), isNonLinearSpace);
		UUID newTextureId = RegisterTexture(newTexture);
		m_CachedTexturePaths.insert({ texturePath, newTextureId });
		return newTextureId;
	}

	UUID TextureLoader::NewUncompressedTextureFromMemory(void* data, IcePickRenderer::TextureSettings textureSettings, bool isNonLinearSpace) {
		if (!data) {
			IP_LOG("Texture has null data.", IP_ERROR_LOG);
			return UUID::Unitialised();
		}

		IP_LOG("Loading a texture from memory not implemented.", IP_WARN_LOG);
		return UUID::Unitialised();
	}

	UUID TextureLoader::NewTextureFromScene(std::string texturePath, const aiScene* scene, bool isNonLinearSpace) {
		// Check if scene texture has been loaded.
		auto iterator = m_CachedSceneTexturePaths.find(texturePath);
		if (iterator != m_CachedSceneTexturePaths.end())
			return iterator->second;

		if (texturePath[0] != '*') { // External texture file
			return NewTextureFromFile(texturePath, isNonLinearSpace);
		}

		int texIndex = std::atoi(texturePath.c_str() + 1);
		IP_ASSERT((texIndex >= 0) && (texIndex < scene->mNumTextures), "Invalid scene texture index.");

		const aiTexture* tex = scene->mTextures[texIndex];
		UUID newTextureId;
		if (tex->mHeight == 0) { // Compressed embedded texture
			IcePickRenderer::Texture newTexture(reinterpret_cast<unsigned char*>(tex->pcData), tex->mWidth, isNonLinearSpace);
			newTextureId = RegisterTexture(newTexture);
		}
		else { // Raw uncompressed embedded texture
			const int numTextureChannels = 4;
			IcePickRenderer::Texture newTexture(reinterpret_cast<unsigned char*>(tex->pcData), tex->mWidth, tex->mHeight, numTextureChannels, isNonLinearSpace);
			newTextureId = RegisterTexture(newTexture);
		}

		m_CachedSceneTexturePaths.insert({ texturePath, newTextureId });
		return newTextureId;
	}

	UUID TextureLoader::NewTextureFromAsset(std::filesystem::path& assetPath) {
		using nlohmann::json;
		std::ifstream jsonFileStream(assetPath);

		if (jsonFileStream.fail())
			return UUID::Unitialised();

		json assetFile = json::parse(jsonFileStream);

		std::string assetVersion = assetFile.value("version", "0.0");
		uint64_t textureId = JsonUtils::GetUint64(assetFile, "ID");
		std::string sourcePath = assetFile.value("sourcePath", "");
		bool isNonLinearSpace = assetFile.value("sRGB", false);

		jsonFileStream.close();

		SetLoaderBasePath(assetPath.parent_path());
		bool newTextureCreated = NewTextureFromFileWithID(sourcePath, textureId, isNonLinearSpace);

		if (newTextureCreated)
			m_CachedTextureAssetPaths.insert({ assetPath, textureId });

		CleanUpAfterLoad();
		return UUID{ textureId };
	}

	const IcePickRenderer::Texture& TextureLoader::GetTexture(UUID id) {
		if (id == UUID::Unitialised() || id == m_DefaultTextureId)
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

	const IcePickRenderer::Texture& TextureLoader::GetDefaultTexture() {
		return m_DefaultTexture;
	}

	const UUID TextureLoader::GetDefaultTextureID()	{
		return m_DefaultTextureId;
	}

	void TextureLoader::SetLoaderBasePath(std::filesystem::path filePath) {
		m_BaseFilePath = filePath;
	}

	void TextureLoader::DestroyTextureById(UUID textureId) {
		if (textureId == UUID::Unitialised() || textureId == m_DefaultTextureId)
			return;

		InvalidateCache();

		auto iterator = m_LoadedTextures.find(textureId);
		if (iterator != m_LoadedTextures.end()) {
			iterator->second.Destroy();
			m_LoadedTextures.erase(iterator);
		}

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

	bool TextureLoader::NewTextureFromFileWithID(std::filesystem::path texturePath, UUID textureId, bool isNonLinearSpace)	{
		std::error_code errorCode;
		texturePath = std::filesystem::canonical(m_BaseFilePath / texturePath, errorCode); // resolve symlinks and relative paths.
		if (errorCode) {
			IP_LOG("Error resolving path: " + texturePath.string() + ". " + errorCode.message(), IP_ERROR_LOG);
			return false;
		}
		
		if (texturePath == m_DefaultTexturePath)
			return false;

		auto iterator = m_CachedTexturePaths.find(texturePath);
		// Texture has already been loaded.
		if (iterator != m_CachedTexturePaths.end()) {
			return false;
		}

		IcePickRenderer::Texture newTexture(texturePath.string(), isNonLinearSpace);
		if (!newTexture.IsValid())
			return false;

		m_LoadedTextures.insert({ textureId, newTexture });
		m_CachedTexturePaths.insert({ texturePath, textureId });
		return true;
	}

	TextureLoader::~TextureLoader() {
		ShutDown();
	}
}