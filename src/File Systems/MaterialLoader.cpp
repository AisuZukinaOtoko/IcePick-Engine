#include "MaterialLoader.h"
#include "../LogSystem.h"
#include "../Utilities/Assert.h"
#include "../Utilities/JsonUtils.h"
#include "json.hpp"
#include <fstream>

namespace IcePick {
	MaterialLoader::MaterialLoader() {
		/*m_DefaultMaterial.ShaderID = UUID::Unitialised();
		m_DefaultMaterial.AlbedoMap = 1;
		m_DefaultMaterial.NormalMap = 1;
		m_DefaultMaterial.RoughnessMap = 1;
		m_DefaultMaterial.MetallicMap = 1;
		m_DefaultMaterial.EmissiveMap = 1;
		m_DefaultMaterial.AlbedoColour = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
		m_DefaultMaterial.SpecularColour = glm::vec3(0.0f, 1.0f, 1.0f);
		m_DefaultMaterial.SampleBitmask = 0;*/
		//m_DefaultMaterial.SampleBitmask |= MaterialAsset::ALBEDO;
		//m_CachedMaterial = m_DefaultMaterial;
		m_DefaultMaterialTextureSamplerIdentifiers.reserve(MaterialTextureTypes::TYPE_COUNT);

		m_DefaultMaterialTextureSamplerIdentifiers.emplace_back("u_AlbedoTexUnit");
	}

	void MaterialLoader::InvalidateCache() {
		//m_CachedMaterialId = UUID::Unitialised();
		//m_CachedMaterial = m_DefaultMaterial;

		m_CachedMaterialBaseId = UUID::Unitialised();
		m_CachedMaterialInstanceId = UUID::Unitialised();

		m_CachedMaterialBase = m_DefaultMaterialBase;
		m_CachedMaterialInstance = m_DefaultMaterialInstance;
	}

	//void MaterialLoader::ConstructMaterial(const MaterialAsset& mat, Material& result, TextureLoader& textureLoader, ShaderLoader& shaderLoader) const {
	//	// Retrieving Texture IDs is expensive, so the check is necessary.
	//	// We don't reset the mat.TextureID in case the user wants to re-activate(sample) it

	//	// Albedo map
	//	if (mat.SampleBitmask & MaterialAsset::ALBEDO)
	//		result.AlbedoMap = textureLoader.GetTexture(mat.AlbedoTexture).GetID();
	//	else
	//		result.AlbedoMap = textureLoader.GetDefaultTexture().GetID();

	//	// Normal map
	//	if (mat.SampleBitmask & MaterialAsset::NORMAL)
	//		result.NormalMap = textureLoader.GetTexture(mat.NormalTexture).GetID();
	//	else
	//		result.NormalMap = textureLoader.GetDefaultTexture().GetID();

	//	// Roughness map
	//	if (mat.SampleBitmask & MaterialAsset::ROUGHNESS)
	//		result.RoughnessMap = textureLoader.GetTexture(mat.RoughnessTexture).GetID();
	//	else
	//		result.RoughnessMap = textureLoader.GetDefaultTexture().GetID();

	//	// Metallic map
	//	if (mat.SampleBitmask & MaterialAsset::METTALIC)
	//		result.MetallicMap = textureLoader.GetTexture(mat.MetallicTexture).GetID();
	//	else
	//		result.MetallicMap = textureLoader.GetDefaultTexture().GetID();

	//	// Emissive map
	//	if (mat.SampleBitmask & MaterialAsset::EMISSIVE)
	//		result.EmissiveMap = textureLoader.GetTexture(mat.EmissiveTexture).GetID();
	//	else
	//		result.EmissiveMap = textureLoader.GetDefaultTexture().GetID();

	//	result.SampleBitmask = mat.SampleBitmask;
	//	ShaderProgram& materialShader = shaderLoader.GetShaderProgram(mat.ShaderID);
	//	result.ShaderID = materialShader.GetID();

	//	result.AlbedoColour = mat.AlbedoColour;
	//	result.SpecularColour = mat.SpecularColour;
	//	result.RoughnessValue = mat.RoughnessValue;
	//	result.MetalValue = mat.MetalValue;
	//	result.EmissiveValue = mat.EmissiveValue;
	//}

	/*UUID MaterialLoader::RegisterMaterialAsset(const MaterialAsset& materialAsset) {
		UUID materialId;
		m_LoadedMaterialAssets.insert({ materialId, materialAsset });
		return materialId;
	}*/

	UUID MaterialLoader::RegisterMaterialBase(const MaterialBase& materialBase) {
		UUID materialBaseId;
		m_LoadedMaterialBases.insert({ materialBaseId , materialBase });
		return materialBaseId;
	}

	UUID MaterialLoader::RegisterMaterialInstance(const MaterialInstance& materialInstance) {
		UUID materialInsatnceId;
		m_LoadedMaterialInstances.insert({ materialInsatnceId , materialInstance });
		return materialInsatnceId;
	}

	/*Material MaterialLoader::GetMaterial(UUID id, TextureLoader& textureLoader, ShaderLoader& shaderLoader) {
		if (id == UUID::Unitialised())
			return m_DefaultMaterial;

		if (id == m_CachedMaterialId)
			return m_CachedMaterial;

		m_CachedMaterialId = id;
		auto iterator = m_LoadedMaterialAssets.find(id);
		if (iterator == m_LoadedMaterialAssets.end()) {
			m_CachedMaterial = m_DefaultMaterial;
			return m_DefaultMaterial;
		}

		Material resultMaterial;
		ConstructMaterial(iterator->second, resultMaterial, textureLoader, shaderLoader);
		m_CachedMaterial = resultMaterial;
		return resultMaterial;
		
	}*/

	/*MaterialAsset& MaterialLoader::GetMaterialAsset(UUID Id) {
		if (Id == UUID::Unitialised())
			return m_DefaultMaterial;

		if (Id == m_CachedMaterialId)
			return m_CachedMaterial;

		m_CachedMaterialId = Id;
		auto iterator = m_LoadedMaterialAssets.find(Id);
		if (iterator == m_LoadedMaterialAssets.end()) {
			m_CachedMaterial = m_DefaultMaterial;
			return m_DefaultMaterial;
		}

		m_CachedMaterial = iterator->second;
		return iterator->second;
	}*/

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

	/*void MaterialLoader::SetDefaultMaterial(Material defaultMaterial) {
		m_DefaultMaterial = defaultMaterial;
		InvalidateCache();
	}*/

	//void MaterialLoader::UpdateMaterial(UUID materialId, const MaterialAsset& newMaterial) {
		/*auto materialIterator = m_LoadedMaterialAssets.find(materialId);
		if (materialIterator == m_LoadedMaterialAssets.end())
			return;

		MaterialAsset& oldMaterial = materialIterator->second;
		oldMaterial = newMaterial;
		InvalidateCache();*/
	//}

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
		IP_ASSERT(materialIndex <= numMaterials, "Material index out of bounds.");
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

		//const unsigned int numMaterials = scene->mNumMaterials;
		//IP_ASSERT(materialIndex <= numMaterials, "Material index out of bounds.");

		//aiMaterial* mat = scene->mMaterials[materialIndex];
		
		MaterialInstance newMaterialInstance;
		SetMaterialInstanceBaseTextureDataFromScene(newMaterialInstance, MaterialTextureTypes::DIFFUSE_TEXTURE, scene, materialIndex, textureLoader);
		//UUID diffuseTextureId = GetSceneMaterialTexture(scene, aiTextureType_DIFFUSE, mat, textureLoader);
		//newMaterialInstance.InstanceTextureData.emplace_back(m_DefaultMaterialBase.Id, diffuseTextureId);
		//MaterialAsset newMaterial;
		//newMaterial.ShaderID = m_LoadMaterialShaderId;
		//newMaterial.AlbedoTexture = GetSceneMaterialTexture(scene, aiTextureType_DIFFUSE, mat, textureLoader);
		//newMaterial.NormalTexture = GetSceneMaterialTexture(scene, aiTextureType_NORMALS, mat, textureLoader);
		//newMaterial.RoughnessTexture = GetSceneMaterialTexture(scene, aiTextureType_SHININESS, mat, textureLoader);
		//newMaterial.MetallicTexture = GetSceneMaterialTexture(scene, aiTextureType_METALNESS, mat, textureLoader);
		//newMaterial.EmissiveTexture = GetSceneMaterialTexture(scene, aiTextureType_EMISSIVE, mat, textureLoader);

		//GetSceneMaterialColours(newMaterial, mat);
		//SetMaterialSampleBits(newMaterial, UUID::Unitialised());

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

		if (jsonFileStream.fail())
			return UUID::Unitialised();

		MaterialBase loadMaterialBase;
		json assetFile = json::parse(jsonFileStream);

		std::string assetVersion = assetFile.value("version", "0.0");


		uint64_t materialId = JsonUtils::GetUint64(assetFile, "ID");
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

		/*{
			"version": "0.1",
				"ID" : 420,
				"baseId" : 69,
				"textureParameters" : [
			{
				"Id": 2,
					"baseDataId" : 2,
					"textureId" : 2
			}
				]
		}*/


		std::ifstream jsonFileStream(assetPath);

		if (jsonFileStream.fail())
			return UUID::Unitialised();

		MaterialInstance loadMaterialInstance;
		json assetFile = json::parse(jsonFileStream);

		std::string assetVersion = assetFile.value("version", "0.0");


		uint64_t materialInstanceId = JsonUtils::GetUint64(assetFile, "ID");
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

	// Only called when loading a new material
	// Unitialised material texture slots will not be sampled within the shader
	/*void MaterialLoader::SetMaterialSampleBits(MaterialAsset& materialAsset, UUID defualtTextureId) {
		if (materialAsset.AlbedoTexture != defualtTextureId)
			materialAsset.SampleBitmask |= MaterialAsset::ALBEDO;

		if (materialAsset.NormalTexture != defualtTextureId)
			materialAsset.SampleBitmask |= MaterialAsset::NORMAL;

		if (materialAsset.RoughnessTexture != defualtTextureId)
			materialAsset.SampleBitmask |= MaterialAsset::ROUGHNESS;

		if (materialAsset.MetallicTexture != defualtTextureId)
			materialAsset.SampleBitmask |= MaterialAsset::METTALIC;

		if (materialAsset.EmissiveTexture != defualtTextureId)
			materialAsset.SampleBitmask |= MaterialAsset::EMISSIVE;
	}*/

	void MaterialLoader::CleanUpAfterLoad()	{
		m_CachedSceneMaterialInstances.clear();
	}

	void MaterialLoader::ShutDown(TextureLoader& textureLoader) {

	}

	MaterialLoader::~MaterialLoader() {

	}
	
}