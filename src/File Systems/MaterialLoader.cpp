#include "MaterialLoader.h"
#include "../LogSystem.h"
#include "../Utilities/Assert.h"
#include "../Utilities/JsonUtils.h"
#include "json.hpp"
#include <fstream>

namespace IcePick {
	MaterialLoader::MaterialLoader() {
		m_DefaultMaterialTextureSamplerIdentifiers.reserve(MaterialTextureTypes::TYPE_COUNT);
		m_DefaultMaterialTextureSamplerIdentifiers.emplace_back("u_AlbedoTexUnit");

		UUID baseTextureDataId{};
		m_DefaultMaterialBase.MaterialTextures.push_back({ baseTextureDataId, m_DefaultMaterialTextureSamplerIdentifiers[DIFFUSE_TEXTURE] });
		m_DefaultMaterialInstance.MaterialBaseId = m_DefaultMaterialBase.Id;
	}

	void MaterialLoader::InvalidateCache() {
		m_CachedMaterialBaseId = UUID::Unitialised();
		m_CachedMaterialInstanceId = UUID::Unitialised();

		m_CachedMaterialBase = m_DefaultMaterialBase;
		m_CachedMaterialInstance = m_DefaultMaterialInstance;
	}

	UUID MaterialLoader::RegisterMaterialBase(const MaterialBase& materialBase) {
		m_LoadedMaterialBases.insert({ materialBase.Id , materialBase });
		return materialBase.Id;
	}

	UUID MaterialLoader::RegisterMaterialInstance(const MaterialInstance& materialInstance) {
		m_LoadedMaterialInstances.insert({ materialInstance.Id , materialInstance });
		return materialInstance.Id;
	}

	MaterialBase& MaterialLoader::GetMaterialBase(UUID Id) {
		if (Id == UUID::Unitialised())
			return m_DefaultMaterialBase;
		
		if (Id == m_CachedMaterialBaseId)
			return m_CachedMaterialBase;

		m_CachedMaterialBaseId = Id;
		auto iterator = m_LoadedMaterialBases.find(Id);
		if (iterator == m_LoadedMaterialBases.end()) {
			m_CachedMaterialBase = m_DefaultMaterialBase;
			return m_DefaultMaterialBase;
		}

		m_CachedMaterialBase = iterator->second;
		return iterator->second;
	}

	MaterialInstance& MaterialLoader::GetMaterialInstance(UUID Id) {
		if (Id == UUID::Unitialised())
			return m_DefaultMaterialInstance;
		
		if (Id == m_CachedMaterialInstanceId)
			return m_CachedMaterialInstance;

		m_CachedMaterialInstanceId = Id;
		auto iterator = m_LoadedMaterialInstances.find(Id);
		if (iterator == m_LoadedMaterialInstances.end()) {
			m_CachedMaterialInstance = m_DefaultMaterialInstance;
			return m_DefaultMaterialInstance;
		}

		m_CachedMaterialInstance = iterator->second;
		return iterator->second;
	}

	void MaterialLoader::SetLoadMaterialShaderID(UUID materialShaderId)	{
		m_LoadMaterialShaderId = materialShaderId;
	}

	void MaterialLoader::UpdateMaterialBase(UUID Id, const MaterialBase& newMaterialBase) {
		auto materialBaseIterator = m_LoadedMaterialBases.find(Id);

		if (materialBaseIterator == m_LoadedMaterialBases.end()) {
			IP_LOG("Cannot update material base. Base does not exist.", IP_WARN_LOG);
			return;
		}

		MaterialBase& materialBase = materialBaseIterator->second;
		materialBase = newMaterialBase;
		InvalidateCache();
	}

	void MaterialLoader::UpdateMaterialInstance(UUID Id, const MaterialInstance& newMaterialInstance) {
		auto materialInstanceIterator = m_LoadedMaterialInstances.find(Id);

		if (materialInstanceIterator == m_LoadedMaterialInstances.end()) {
			IP_LOG("Cannot update material instance. Instance does not exist.", IP_WARN_LOG);
			return;
		}

		MaterialInstance& materialInstance = materialInstanceIterator->second;
		materialInstance = newMaterialInstance;
		InvalidateCache();
	}

	UUID MaterialLoader::GetSceneMaterialTexture(const aiScene* scene, aiTextureType textureType, aiMaterial* mat, TextureLoader& textureLoader) {
		// IcePick only supports 1 texture of each type, so index is hardcoded.
		const unsigned int textureIndex = 0;
		aiString t;
		aiReturn hasTextureOfThatType = mat->GetTexture(textureType, textureIndex, &t);
		std::string texturePath = t.C_Str();

		if (hasTextureOfThatType != aiReturn_SUCCESS) {
			return UUID::Unitialised();
		}

		return textureLoader.NewTextureFromScene(texturePath, scene);
	}

	//void MaterialLoader::GetSceneMaterialColours(MaterialAsset& materialAsset, aiMaterial* mat) {
		/*aiColor4D materialColour;
		if (aiGetMaterialColor(mat, AI_MATKEY_BASE_COLOR, &materialColour) == aiReturn_SUCCESS) {
			materialAsset.AlbedoColour = glm::vec4(materialColour.r, materialColour.g, materialColour.b, materialColour.a);
		}
		else if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &materialColour) == aiReturn_SUCCESS) {
			materialAsset.AlbedoColour = glm::vec4(materialColour.r, materialColour.g, materialColour.b, materialColour.a);
		}
		else {
			materialAsset.AlbedoColour = m_DefaultMaterial.AlbedoColour;
		}

		if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &materialColour) == aiReturn_SUCCESS) {
			materialAsset.SpecularColour = glm::vec3(materialColour.r, materialColour.g, materialColour.b);
		}
		else {
			materialAsset.SpecularColour = m_DefaultMaterial.SpecularColour;
		}
		*/
	//}

	void MaterialLoader::SetMaterialInstanceBaseTextureDataFromScene(MaterialInstance& materialInstance, MaterialTextureTypes textureType, const aiScene* scene, unsigned int materialIndex, TextureLoader& textureLoader) {
		const unsigned int numMaterials = scene->mNumMaterials;
		IP_ASSERT(materialIndex < numMaterials, "Material index out of bounds.");
		aiMaterial* sceneMaterial = scene->mMaterials[materialIndex];
		
		aiTextureType sceneTextureType;
		std::string textureSamplerIdentifier = m_DefaultMaterialTextureSamplerIdentifiers[textureType];
		UUID materialBaseTextureDataId = UUID::Unitialised();

		switch (textureType) {
		case MaterialTextureTypes::DIFFUSE_TEXTURE:
			sceneTextureType = aiTextureType_DIFFUSE;
			break;
		default:
			sceneTextureType = aiTextureType_DIFFUSE;
		}

		const MaterialBase& materialBase = GetMaterialBase(materialInstance.MaterialBaseId);
		for (const auto& materialBaseTextureData : materialBase.MaterialTextures) {
			if (materialBaseTextureData.SamplerIdentifier == textureSamplerIdentifier) {
				materialBaseTextureDataId = materialBaseTextureData.Id;
				break;
			}
		}

		UUID materialInstanceTextureId = GetSceneMaterialTexture(scene, sceneTextureType, sceneMaterial, textureLoader);
		materialInstance.InstanceTextureData.emplace_back(materialBaseTextureDataId, materialInstanceTextureId);
	}

	UUID MaterialLoader::NewMaterialInstanceFromScene(const aiScene* scene, unsigned int materialIndex, TextureLoader& textureLoader) {
		// Check if scene material has been loaded before.
		auto iterator = m_CachedSceneMaterialInstances.find(materialIndex);
		if (iterator != m_CachedSceneMaterialInstances.end())
			return iterator->second;
		
		MaterialInstance newMaterialInstance;
		newMaterialInstance.MaterialBaseId = m_DefaultMaterialInstance.MaterialBaseId;
		SetMaterialInstanceBaseTextureDataFromScene(newMaterialInstance, MaterialTextureTypes::DIFFUSE_TEXTURE, scene, materialIndex, textureLoader);

		UUID newMaterialInstanceId = RegisterMaterialInstance(newMaterialInstance);
		m_CachedSceneMaterialInstances.insert({ materialIndex, newMaterialInstanceId });
		return newMaterialInstanceId;
	}

	UUID MaterialLoader::NewMaterialBaseFromCopy(const MaterialBase& newMaterialBase) {
		return RegisterMaterialBase(newMaterialBase);
	}

	UUID MaterialLoader::NewMaterialInstanceFromCopy(const MaterialInstance& newMaterialInstance) {
		return RegisterMaterialInstance(newMaterialInstance);
	}

	UUID MaterialLoader::NewMaterialBaseFromAsset(std::filesystem::path& assetPath) {
		using nlohmann::json;

		const auto iterator = m_CachedMaterialBaseAssetPaths.find(assetPath);
		if (iterator != m_CachedMaterialBaseAssetPaths.end()) // asset already loaded.
			return iterator->second;

		std::ifstream jsonFileStream(assetPath);

		if (jsonFileStream.fail()) {
			IP_LOG("Failed to load material base: " + assetPath.string() + ".", IP_ERROR_LOG);
			return UUID::Unitialised();
		}

		MaterialBase loadMaterialBase;
		json assetFile = json::parse(jsonFileStream);

		int assetVersion = assetFile.value("version", 0);
		if (assetVersion < m_LoaderVersion) {
			IP_LOG("Material base, " + assetPath.string() + ", is an outdated version. Loaded data may be incorrect.", IP_WARN_LOG);
		}

		uint64_t materialId = JsonUtils::GetUint64(assetFile, "Id");
		uint64_t shaderId = JsonUtils::GetUint64(assetFile, "shaderId");
		uint64_t graphId = JsonUtils::GetUint64(assetFile, "graphId");

		UUID Id{ materialId };
		loadMaterialBase.Id = Id;
		loadMaterialBase.ShaderId = UUID{ shaderId };
		loadMaterialBase.ShaderGraphId = UUID{ graphId };

		if (assetFile.contains("textureParameters") && assetFile["textureParameters"].is_array()) {
			json& materialTextureParameters = assetFile["textureParameters"];
			
			for (auto textureIterator = materialTextureParameters.begin(); textureIterator != materialTextureParameters.end(); textureIterator++) {
				MaterialBaseTextureData& materialBaseTextureData = loadMaterialBase.MaterialTextures.emplace_back();
				materialBaseTextureData.Id = JsonUtils::GetUint64(*textureIterator, "Id");
				materialBaseTextureData.SamplerIdentifier = textureIterator->value("sampler", "none");
			}
		}

		jsonFileStream.close();

		m_LoadedMaterialBases.insert({ Id, loadMaterialBase });
		m_CachedMaterialBaseAssetPaths.insert({ assetPath, Id });

		return Id;
	}

	UUID MaterialLoader::NewMaterialInstanceFromAsset(std::filesystem::path& assetPath) {
		using nlohmann::json;

		const auto iterator = m_CachedMaterialInstanceAssetPaths.find(assetPath);
		if (iterator != m_CachedMaterialInstanceAssetPaths.end()) // asset already loaded.
			return iterator->second;

		std::ifstream jsonFileStream(assetPath);

		if (jsonFileStream.fail()) {
			IP_LOG("Failed to load material instance: " + assetPath.string() + ".", IP_ERROR_LOG);
			return UUID::Unitialised();
		}

		MaterialInstance loadMaterialInstance;
		json assetFile = json::parse(jsonFileStream);

		int assetVersion = assetFile.value("version", 0);
		if (assetVersion < m_LoaderVersion) {
			IP_LOG("Material instance, " + assetPath.string() + ", is an outdated version. Loaded data may be incorrect.", IP_WARN_LOG);
		}

		uint64_t materialInstanceId = JsonUtils::GetUint64(assetFile, "Id");
		uint64_t materialBaseId = JsonUtils::GetUint64(assetFile, "baseId");

		UUID Id{ materialInstanceId };
		loadMaterialInstance.Id = Id;
		loadMaterialInstance.MaterialBaseId = UUID{ materialBaseId };

		if (assetFile.contains("textureParameters") && assetFile["textureParameters"].is_array()) {
			json& materialTextureParameters = assetFile["textureParameters"];

			for (auto textureIterator = materialTextureParameters.begin(); textureIterator != materialTextureParameters.end(); textureIterator++) {
				UUID instanceParameterDataId = JsonUtils::GetUint64(*textureIterator, "Id");
				UUID instanceParameterTextureBaseDataId = JsonUtils::GetUint64(*textureIterator, "baseDataId");
				UUID instanceParameterTextureId = JsonUtils::GetUint64(*textureIterator, "textureId");
				MaterialInstanceData<UUID>& instanceData = loadMaterialInstance.InstanceTextureData.emplace_back(instanceParameterTextureBaseDataId, instanceParameterTextureId);
				instanceData.Id = instanceParameterDataId;
			}
		}

		jsonFileStream.close();

		m_LoadedMaterialInstances.insert({ Id, loadMaterialInstance });
		m_CachedMaterialInstanceAssetPaths.insert({ assetPath, Id });

		return Id;
	}

	void MaterialLoader::SaveMaterialBase(std::filesystem::path assetPath, const MaterialBase& materialBase) {
		nlohmann::json json;

		json["version"] = m_LoaderVersion;
		json["Id"] = static_cast<uint64_t>(materialBase.Id);
		json["shaderId"] = static_cast<uint64_t>(materialBase.ShaderId);
		json["graphId"] = static_cast<uint64_t>(materialBase.ShaderGraphId);

		nlohmann::json textureParameters = nlohmann::json::array();
		for (const auto& materialBaseTexture : materialBase.MaterialTextures) {
			nlohmann::json materialBaseTextureJson;
			materialBaseTextureJson["Id"] = static_cast<uint64_t>(materialBaseTexture.Id);
			materialBaseTextureJson["sampler"] = materialBaseTexture.SamplerIdentifier;
			textureParameters.push_back(materialBaseTextureJson);
		}
		json["textureParameters"] = textureParameters;

		std::ofstream outFile(assetPath);
		if (outFile.is_open()) {
			outFile << std::setw(4) << json; // pretty print
		}
		else {
			IP_LOG("Failed to save material base: " + assetPath.string(), IP_ERROR_LOG);
		}
	}

	void MaterialLoader::SaveMaterialInstance(std::filesystem::path assetPath, const MaterialInstance& materialInstance) {
		nlohmann::json json;

		json["version"] = m_LoaderVersion;
		json["Id"] = static_cast<uint64_t>(materialInstance.Id);
		json["baseId"] = static_cast<uint64_t>(materialInstance.MaterialBaseId);

		nlohmann::json textureParameters = nlohmann::json::array();
		for (const auto& materialInstanceTextureData : materialInstance.InstanceTextureData) {
			nlohmann::json materialBaseTextureJson;
			materialBaseTextureJson["Id"] = static_cast<uint64_t>(materialInstanceTextureData.Id);
			materialBaseTextureJson["baseDataId"] = static_cast<uint64_t>(materialInstanceTextureData.MaterialBaseDataId);
			materialBaseTextureJson["textureId"] = static_cast<uint64_t>(materialInstanceTextureData.Data);
			textureParameters.push_back(materialBaseTextureJson);
		}
		json["textureParameters"] = textureParameters;

		std::ofstream outFile(assetPath);
		if (outFile.is_open()) {
			outFile << std::setw(4) << json; // pretty print
		}
		else {
			IP_LOG("Failed to save material instance: " + assetPath.string(), IP_ERROR_LOG);
		}
	}

	void MaterialLoader::CleanUpAfterLoad()	{
		m_CachedSceneMaterialInstances.clear();
	}

	void MaterialLoader::ShutDown(TextureLoader& textureLoader) {

	}

	MaterialLoader::~MaterialLoader() {

	}
	
}