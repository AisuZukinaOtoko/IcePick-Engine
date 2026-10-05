#include "MaterialLoader.h"
#include "TextureLoader.h"
#include "ShaderLoader.h"
#include "ImportSettings.h"

#include "../LogSystem.h"
#include "../Utilities/Assert.h"
#include "../Utilities/JsonUtils.h"
#include <nlohmann/json.hpp>
#include <fstream>


namespace IcePick {
	MaterialLoader::MaterialLoader() {
		m_DefaultMaterialTextureSamplerIdentifiers.reserve(MaterialTextureTypes::TYPE_COUNT);
		m_DefaultMaterialTextureSamplerIdentifiers.emplace_back("u_AlbedoTexUnit");

		UUID baseTextureDataId{};
		m_DefaultMaterialBase.MaterialTextures.push_back({ "Diffuse Texture", m_DefaultMaterialTextureSamplerIdentifiers[DIFFUSE_TEXTURE], baseTextureDataId });
		m_DefaultMaterialInstance.MaterialBaseId = m_DefaultMaterialBase.Id;
	}

	void MaterialLoader::Init(ShaderLoader& shaderLoader) {
		MaterialBase defaultSkinnedMeshMaterialBase;
		UUID baseTextureDataId{};
		defaultSkinnedMeshMaterialBase.MaterialTextures.push_back({ "Diffuse Texture", m_DefaultMaterialTextureSamplerIdentifiers[DIFFUSE_TEXTURE], baseTextureDataId });

		ShaderSource defaultSkinnedShaderSource;
		defaultSkinnedShaderSource.VertexShaderSource = shaderLoader.LoadFile("Game Engine/res/Shaders/skinning.vert.shader", 0);
		defaultSkinnedShaderSource.FragmentShaderSource = shaderLoader.LoadFile("Game Engine/res/Shaders/default.frag.shader", 0);
		defaultSkinnedMeshMaterialBase.ShaderId = shaderLoader.CreateShaderProgram(defaultSkinnedShaderSource);
		m_LoadSkinnedMeshMaterialBaseId = RegisterMaterialBase(defaultSkinnedMeshMaterialBase);
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

	UUID MaterialLoader::GetSceneMaterialTexture(const aiScene* scene, aiTextureType textureType, bool isNonLinearSpace, aiMaterial* mat, TextureLoader& textureLoader) {
		// IcePick only supports 1 texture of each type, so index is hardcoded.
		const unsigned int textureIndex = 0;
		aiString t;
		aiReturn hasTextureOfThatType = mat->GetTexture(textureType, textureIndex, &t);
		std::string texturePath = t.C_Str();

		if (hasTextureOfThatType != aiReturn_SUCCESS) {
			return UUID::Unitialised();
		}

		return textureLoader.NewTextureFromScene(texturePath, scene, isNonLinearSpace);
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

	void MaterialLoader::SetMaterialInstanceBaseTextureDataFromScene(MaterialInstance& materialInstance, MaterialTextureTypes textureType, const aiScene* scene, unsigned int materialIndex, TextureLoader& textureLoader, const ImportSettings& importSettings) {
		const unsigned int numMaterials = scene->mNumMaterials;
		IP_ASSERT(materialIndex < numMaterials, "Material index out of bounds.");
		aiMaterial* sceneMaterial = scene->mMaterials[materialIndex];
		
		aiTextureType sceneTextureType;
		bool textureIsNonLinear = false;
		std::string textureSamplerIdentifier = m_DefaultMaterialTextureSamplerIdentifiers[textureType];
		UUID materialBaseTextureDataId = UUID::Unitialised();

		switch (textureType) {
		case MaterialTextureTypes::DIFFUSE_TEXTURE:
			sceneTextureType = aiTextureType_DIFFUSE;
			textureIsNonLinear = importSettings.LoadDiffuseTextureAsSRGB;
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

		UUID materialInstanceTextureId = GetSceneMaterialTexture(scene, sceneTextureType, textureIsNonLinear, sceneMaterial, textureLoader);
		materialInstance.InstanceTextureData.emplace_back(materialBaseTextureDataId, materialInstanceTextureId);
	}

	UUID MaterialLoader::NewMaterialInstanceFromScene(const aiScene* scene, unsigned int materialIndex, TextureLoader& textureLoader, const ImportSettings& importSettings) {
		// Check if scene material has been loaded before.
		auto iterator = m_CachedSceneMaterialInstances.find(materialIndex);
		if (iterator != m_CachedSceneMaterialInstances.end())
			return iterator->second;
		
		MaterialInstance newMaterialInstance;
		switch (importSettings.LoadMeshAs) {
		case ImportSettings::MeshType::STATIC_MESH:
		{
			newMaterialInstance.MaterialBaseId = m_DefaultMaterialInstance.MaterialBaseId;
			break;
		}
		case ImportSettings::MeshType::SKELETAL_MESH:
		{
			newMaterialInstance.MaterialBaseId = m_LoadSkinnedMeshMaterialBaseId;
			break;
		}
		}

		SetMaterialInstanceBaseTextureDataFromScene(newMaterialInstance, MaterialTextureTypes::DIFFUSE_TEXTURE, scene, materialIndex, textureLoader, importSettings);

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

	UUID MaterialLoader::NewMaterialBaseFromAsset(std::filesystem::path& assetPath, ShaderLoader& shaderLoader) {
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
				materialBaseTextureData.DisplayName = textureIterator->value("displayName", "Texture");
			}
		}

		if (assetFile.contains("floatParameters") && assetFile["floatParameters"].is_array()) {
			json& materialFloatParameters = assetFile["floatParameters"];

			for (auto floatIterator = materialFloatParameters.begin(); floatIterator != materialFloatParameters.end(); floatIterator++) {
				MaterialBaseParameter& materialBaseFloatParameter = loadMaterialBase.MaterialFloatParameters.emplace_back();
				materialBaseFloatParameter.Id = JsonUtils::GetUint64(*floatIterator, "Id");
				materialBaseFloatParameter.ShaderIdentifier = floatIterator->value("shaderIdentifier", "none");
				materialBaseFloatParameter.DisplayName = floatIterator->value("displayName", "Float");
			}
		}

		if (assetFile.contains("vec2Parameters") && assetFile["vec2Parameters"].is_array()) {
			json& materialVec2Parameters = assetFile["vec2Parameters"];

			for (auto vec2Iterator = materialVec2Parameters.begin(); vec2Iterator != materialVec2Parameters.end(); vec2Iterator++) {
				MaterialBaseParameter& materialBaseVec2Parameter = loadMaterialBase.MaterialVec2Parameters.emplace_back();
				materialBaseVec2Parameter.Id = JsonUtils::GetUint64(*vec2Iterator, "Id");
				materialBaseVec2Parameter.ShaderIdentifier = vec2Iterator->value("shaderIdentifier", "none");
				materialBaseVec2Parameter.DisplayName = vec2Iterator->value("displayName", "Vec2");
			}
		}

		if (assetFile.contains("vec3Parameters") && assetFile["vec3Parameters"].is_array()) {
			json& materialVec3Parameters = assetFile["vec3Parameters"];

			for (auto vec3Iterator = materialVec3Parameters.begin(); vec3Iterator != materialVec3Parameters.end(); vec3Iterator++) {
				MaterialBaseParameter& materialBaseVec3Parameter = loadMaterialBase.MaterialVec3Parameters.emplace_back();
				materialBaseVec3Parameter.Id = JsonUtils::GetUint64(*vec3Iterator, "Id");
				materialBaseVec3Parameter.ShaderIdentifier = vec3Iterator->value("shaderIdentifier", "none");
				materialBaseVec3Parameter.DisplayName = vec3Iterator->value("displayName", "Vec3");
			}
		}

		if (assetFile.contains("vec4Parameters") && assetFile["vec4Parameters"].is_array()) {
			json& materialVec4Parameters = assetFile["vec4Parameters"];

			for (auto vec4Iterator = materialVec4Parameters.begin(); vec4Iterator != materialVec4Parameters.end(); vec4Iterator++) {
				MaterialBaseParameter& materialBaseVec4Parameter = loadMaterialBase.MaterialVec4Parameters.emplace_back();
				materialBaseVec4Parameter.Id = JsonUtils::GetUint64(*vec4Iterator, "Id");
				materialBaseVec4Parameter.ShaderIdentifier = vec4Iterator->value("shaderIdentifier", "none");
				materialBaseVec4Parameter.DisplayName = vec4Iterator->value("displayName", "Vec4");
			}
		}

		if (assetFile.contains("colourVec4Parameters") && assetFile["colourVec4Parameters"].is_array()) {
			json& materialColourVec4Parameters = assetFile["colourVec4Parameters"];

			for (auto colourVec4Iterator = materialColourVec4Parameters.begin(); colourVec4Iterator != materialColourVec4Parameters.end(); colourVec4Iterator++) {
				MaterialBaseParameter& materialBaseColourVec4Parameter = loadMaterialBase.MaterialColourVec4Parameters.emplace_back();
				materialBaseColourVec4Parameter.Id = JsonUtils::GetUint64(*colourVec4Iterator, "Id");
				materialBaseColourVec4Parameter.ShaderIdentifier = colourVec4Iterator->value("shaderIdentifier", "none");
				materialBaseColourVec4Parameter.DisplayName = colourVec4Iterator->value("displayName", "Colour");
			}
		}

		json shaderSourceJson = assetFile["shaderSource"];
		ShaderSource shaderSource;
		shaderSource.VertexShaderSource = shaderSourceJson.at("vertex");
		shaderSource.FragmentShaderSource = shaderSourceJson.at("fragment");
		shaderLoader.CreateShaderProgramWithId(shaderSource, shaderId);

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

		if (assetFile.contains("floatParameters") && assetFile["floatParameters"].is_array()) {
			json& materialFloatParameters = assetFile["floatParameters"];

			for (auto floatIterator = materialFloatParameters.begin(); floatIterator != materialFloatParameters.end(); floatIterator++) {
				UUID instanceFloatParameterDataId = JsonUtils::GetUint64(*floatIterator, "Id");
				UUID instanceFloatParameterBaseDataId = JsonUtils::GetUint64(*floatIterator, "baseDataId");
				float instanceParameterFloatValue = floatIterator->value("value", 0.0f);
				MaterialInstanceData<float>& instanceData = loadMaterialInstance.InstanceFloatData.emplace_back(instanceFloatParameterBaseDataId, instanceParameterFloatValue);
				instanceData.Id = instanceFloatParameterDataId;
			}
		}

		if (assetFile.contains("vec2Parameters") && assetFile["vec2Parameters"].is_array()) {
			json& materialVec2Parameters = assetFile["vec2Parameters"];

			for (auto vec2Iterator = materialVec2Parameters.begin(); vec2Iterator != materialVec2Parameters.end(); vec2Iterator++) {
				UUID instanceVec2ParameterDataId = JsonUtils::GetUint64(*vec2Iterator, "Id");
				UUID instanceVec2ParameterBaseDataId = JsonUtils::GetUint64(*vec2Iterator, "baseDataId");

				if (vec2Iterator->contains("value")) {
					nlohmann::json instanceParameterVec2Data = vec2Iterator->at("value");
					glm::vec2 value{ instanceParameterVec2Data.value("x", 0.0f), instanceParameterVec2Data.value("y", 0.0f) };
					MaterialInstanceData<glm::vec2>& instanceData = loadMaterialInstance.InstanceVec2Data.emplace_back(instanceVec2ParameterBaseDataId, value);
					instanceData.Id = instanceVec2ParameterDataId;
				}
				else {
					MaterialInstanceData<glm::vec2>& instanceData = loadMaterialInstance.InstanceVec2Data.emplace_back(instanceVec2ParameterBaseDataId, glm::vec2(0.0f, 0.0f));
					instanceData.Id = instanceVec2ParameterDataId;
				}
				
			}
		}

		if (assetFile.contains("vec3Parameters") && assetFile["vec3Parameters"].is_array()) {
			json& materialVec3Parameters = assetFile["vec3Parameters"];

			for (auto vec3Iterator = materialVec3Parameters.begin(); vec3Iterator != materialVec3Parameters.end(); vec3Iterator++) {
				UUID instanceVec3ParameterDataId = JsonUtils::GetUint64(*vec3Iterator, "Id");
				UUID instanceVec3ParameterBaseDataId = JsonUtils::GetUint64(*vec3Iterator, "baseDataId");

				if (vec3Iterator->contains("value")) {
					nlohmann::json instanceParameterVec3Data = vec3Iterator->at("value");
					glm::vec3 value{ instanceParameterVec3Data.value("x", 0.0f), instanceParameterVec3Data.value("y", 0.0f), instanceParameterVec3Data.value("z", 0.0f) };
					MaterialInstanceData<glm::vec3>& instanceData = loadMaterialInstance.InstanceVec3Data.emplace_back(instanceVec3ParameterBaseDataId, value);
					instanceData.Id = instanceVec3ParameterDataId;
				}
				else {
					MaterialInstanceData<glm::vec3>& instanceData = loadMaterialInstance.InstanceVec3Data.emplace_back(instanceVec3ParameterBaseDataId, glm::vec3(0.0f, 0.0f, 0.0f));
					instanceData.Id = instanceVec3ParameterDataId;
				}

			}
		}

		if (assetFile.contains("vec4Parameters") && assetFile["vec4Parameters"].is_array()) {
			json& materialVec4Parameters = assetFile["vec4Parameters"];

			for (auto vec4Iterator = materialVec4Parameters.begin(); vec4Iterator != materialVec4Parameters.end(); vec4Iterator++) {
				UUID instanceVec4ParameterDataId = JsonUtils::GetUint64(*vec4Iterator, "Id");
				UUID instanceVec4ParameterBaseDataId = JsonUtils::GetUint64(*vec4Iterator, "baseDataId");

				if (vec4Iterator->contains("value")) {
					nlohmann::json instanceParameterVec4Data = vec4Iterator->at("value");
					glm::vec4 value{ instanceParameterVec4Data.value("x", 0.0f), instanceParameterVec4Data.value("y", 0.0f), instanceParameterVec4Data.value("z", 0.0f), instanceParameterVec4Data.value("w", 0.0f) };
					MaterialInstanceData<glm::vec4>& instanceData = loadMaterialInstance.InstanceVec4Data.emplace_back(instanceVec4ParameterBaseDataId, value);
					instanceData.Id = instanceVec4ParameterDataId;
				}
				else {
					MaterialInstanceData<glm::vec4>& instanceData = loadMaterialInstance.InstanceVec4Data.emplace_back(instanceVec4ParameterBaseDataId, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
					instanceData.Id = instanceVec4ParameterDataId;
				}

			}
		}

		if (assetFile.contains("colourVec4Parameters") && assetFile["colourVec4Parameters"].is_array()) {
			json& materialColourVec4Parameters = assetFile["colourVec4Parameters"];

			for (auto colourVec4Iterator = materialColourVec4Parameters.begin(); colourVec4Iterator != materialColourVec4Parameters.end(); colourVec4Iterator++) {
				UUID instanceColourVec4ParameterDataId = JsonUtils::GetUint64(*colourVec4Iterator, "Id");
				UUID instanceColourVec4ParameterBaseDataId = JsonUtils::GetUint64(*colourVec4Iterator, "baseDataId");

				if (colourVec4Iterator->contains("value")) {
					nlohmann::json instanceParameterColourVec4Data = colourVec4Iterator->at("value");
					glm::vec4 value{ instanceParameterColourVec4Data.value("x", 0.0f), instanceParameterColourVec4Data.value("y", 0.0f), instanceParameterColourVec4Data.value("z", 0.0f), instanceParameterColourVec4Data.value("w", 1.0f) };
					MaterialInstanceData<glm::vec4>& instanceData = loadMaterialInstance.InstanceColourVec4Data.emplace_back(instanceColourVec4ParameterBaseDataId, value);
					instanceData.Id = instanceColourVec4ParameterDataId;
				}
				else {
					MaterialInstanceData<glm::vec4>& instanceData = loadMaterialInstance.InstanceColourVec4Data.emplace_back(instanceColourVec4ParameterBaseDataId, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
					instanceData.Id = instanceColourVec4ParameterDataId;
				}

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

		{
			nlohmann::json textureParameters = nlohmann::json::array();
			for (const auto& materialBaseTexture : materialBase.MaterialTextures) {
				nlohmann::json materialBaseTextureJson;
				materialBaseTextureJson["Id"] = static_cast<uint64_t>(materialBaseTexture.Id);
				materialBaseTextureJson["sampler"] = materialBaseTexture.SamplerIdentifier;
				textureParameters.push_back(materialBaseTextureJson);
			}
			json["textureParameters"] = textureParameters;
		}

		{
			nlohmann::json floatParameters = nlohmann::json::array();
			for (const auto& materialBaseFloatParameter : materialBase.MaterialFloatParameters) {
				nlohmann::json materialBaseFloatJson;
				materialBaseFloatJson["Id"] = static_cast<uint64_t>(materialBaseFloatParameter.Id);
				materialBaseFloatJson["shaderIdentifier"] = materialBaseFloatParameter.ShaderIdentifier;
				materialBaseFloatJson["displayName"] = materialBaseFloatParameter.DisplayName;
				floatParameters.push_back(materialBaseFloatJson);
			}
			json["floatParameters"] = floatParameters;
		}

		{
			nlohmann::json vec2Parameters = nlohmann::json::array();
			for (const auto& materialBaseVec2Parameter : materialBase.MaterialVec2Parameters) {
				nlohmann::json materialBaseVec2Json;
				materialBaseVec2Json["Id"] = static_cast<uint64_t>(materialBaseVec2Parameter.Id);
				materialBaseVec2Json["shaderIdentifier"] = materialBaseVec2Parameter.ShaderIdentifier;
				materialBaseVec2Json["displayName"] = materialBaseVec2Parameter.DisplayName;
				vec2Parameters.push_back(materialBaseVec2Json);
			}
			json["vec2Parameters"] = vec2Parameters;
		}

		{
			nlohmann::json vec3Parameters = nlohmann::json::array();
			for (const auto& materialBaseVec3Parameter : materialBase.MaterialVec3Parameters) {
				nlohmann::json materialBaseVec3Json;
				materialBaseVec3Json["Id"] = static_cast<uint64_t>(materialBaseVec3Parameter.Id);
				materialBaseVec3Json["shaderIdentifier"] = materialBaseVec3Parameter.ShaderIdentifier;
				materialBaseVec3Json["displayName"] = materialBaseVec3Parameter.DisplayName;
				vec3Parameters.push_back(materialBaseVec3Json);
			}
			json["vec3Parameters"] = vec3Parameters;
		}

		{
			nlohmann::json vec4Parameters = nlohmann::json::array();
			for (const auto& materialBaseVec4Parameter : materialBase.MaterialVec4Parameters) {
				nlohmann::json materialBaseVec4Json;
				materialBaseVec4Json["Id"] = static_cast<uint64_t>(materialBaseVec4Parameter.Id);
				materialBaseVec4Json["shaderIdentifier"] = materialBaseVec4Parameter.ShaderIdentifier;
				materialBaseVec4Json["displayName"] = materialBaseVec4Parameter.DisplayName;
				vec4Parameters.push_back(materialBaseVec4Json);
			}
			json["vec4Parameters"] = vec4Parameters;
		}

		{
			nlohmann::json colourVec4Parameters = nlohmann::json::array();
			for (const auto& materialBaseColourVec4Parameter : materialBase.MaterialColourVec4Parameters) {
				nlohmann::json materialBaseColourVec4Json;
				materialBaseColourVec4Json["Id"] = static_cast<uint64_t>(materialBaseColourVec4Parameter.Id);
				materialBaseColourVec4Json["shaderIdentifier"] = materialBaseColourVec4Parameter.ShaderIdentifier;
				materialBaseColourVec4Json["displayName"] = materialBaseColourVec4Parameter.DisplayName;
				colourVec4Parameters.push_back(materialBaseColourVec4Json);
			}
			json["colourVec4Parameters"] = colourVec4Parameters;
		}

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

		{
			nlohmann::json textureParameters = nlohmann::json::array();
			for (const auto& materialInstanceTextureData : materialInstance.InstanceTextureData) {
				nlohmann::json materialBaseTextureJson;
				materialBaseTextureJson["Id"] = static_cast<uint64_t>(materialInstanceTextureData.Id);
				materialBaseTextureJson["baseDataId"] = static_cast<uint64_t>(materialInstanceTextureData.MaterialBaseDataId);
				materialBaseTextureJson["textureId"] = static_cast<uint64_t>(materialInstanceTextureData.Data);
				textureParameters.push_back(materialBaseTextureJson);
			}
			json["textureParameters"] = textureParameters;
		}
		
		{
			nlohmann::json floatParameters = nlohmann::json::array();
			for (const auto& materialInstanceFloatData : materialInstance.InstanceFloatData) {
				nlohmann::json materialBaseFloatJson;
				materialBaseFloatJson["Id"] = static_cast<uint64_t>(materialInstanceFloatData.Id);
				materialBaseFloatJson["baseDataId"] = static_cast<uint64_t>(materialInstanceFloatData.MaterialBaseDataId);
				materialBaseFloatJson["value"] = materialInstanceFloatData.Data;
				floatParameters.push_back(materialBaseFloatJson);
			}
			json["floatParameters"] = floatParameters;
		}
		
		{
			nlohmann::json vec2Parameters = nlohmann::json::array();
			for (const auto& materialInstanceVec2Data : materialInstance.InstanceVec2Data) {
				nlohmann::json materialBaseVec2Json;
				materialBaseVec2Json["Id"] = static_cast<uint64_t>(materialInstanceVec2Data.Id);
				materialBaseVec2Json["baseDataId"] = static_cast<uint64_t>(materialInstanceVec2Data.MaterialBaseDataId);

				nlohmann::json vec2Data;
				vec2Data["x"] = materialInstanceVec2Data.Data.x;
				vec2Data["y"] = materialInstanceVec2Data.Data.y;
				materialBaseVec2Json["value"] = vec2Data;
				vec2Parameters.push_back(materialBaseVec2Json);
			}
			json["vec2Parameters"] = vec2Parameters;
		}

		{
			nlohmann::json vec3Parameters = nlohmann::json::array();
			for (const auto& materialInstanceVec3Data : materialInstance.InstanceVec3Data) {
				nlohmann::json materialBaseVec3Json;
				materialBaseVec3Json["Id"] = static_cast<uint64_t>(materialInstanceVec3Data.Id);
				materialBaseVec3Json["baseDataId"] = static_cast<uint64_t>(materialInstanceVec3Data.MaterialBaseDataId);

				nlohmann::json vec3Data;
				vec3Data["x"] = materialInstanceVec3Data.Data.x;
				vec3Data["y"] = materialInstanceVec3Data.Data.y;
				vec3Data["y"] = materialInstanceVec3Data.Data.z;
				materialBaseVec3Json["value"] = vec3Data;
				vec3Parameters.push_back(materialBaseVec3Json);
			}
			json["vec3Parameters"] = vec3Parameters;
		}

		{
			nlohmann::json vec4Parameters = nlohmann::json::array();
			for (const auto& materialInstanceVec4Data : materialInstance.InstanceVec4Data) {
				nlohmann::json materialBaseVec4Json;
				materialBaseVec4Json["Id"] = static_cast<uint64_t>(materialInstanceVec4Data.Id);
				materialBaseVec4Json["baseDataId"] = static_cast<uint64_t>(materialInstanceVec4Data.MaterialBaseDataId);

				nlohmann::json vec4Data;
				vec4Data["x"] = materialInstanceVec4Data.Data.x;
				vec4Data["y"] = materialInstanceVec4Data.Data.y;
				vec4Data["y"] = materialInstanceVec4Data.Data.z;
				vec4Data["w"] = materialInstanceVec4Data.Data.w;
				materialBaseVec4Json["value"] = vec4Data;
				vec4Parameters.push_back(materialBaseVec4Json);
			}
			json["vec4Parameters"] = vec4Parameters;
		}

		{
			nlohmann::json colourVec4Parameters = nlohmann::json::array();
			for (const auto& materialInstanceColourVec4Data : materialInstance.InstanceColourVec4Data) {
				nlohmann::json materialBaseColourVec4Json;
				materialBaseColourVec4Json["Id"] = static_cast<uint64_t>(materialInstanceColourVec4Data.Id);
				materialBaseColourVec4Json["baseDataId"] = static_cast<uint64_t>(materialInstanceColourVec4Data.MaterialBaseDataId);

				nlohmann::json colourVec4Data;
				colourVec4Data["x"] = materialInstanceColourVec4Data.Data.x;
				colourVec4Data["y"] = materialInstanceColourVec4Data.Data.y;
				colourVec4Data["y"] = materialInstanceColourVec4Data.Data.z;
				colourVec4Data["w"] = materialInstanceColourVec4Data.Data.w;
				materialBaseColourVec4Json["value"] = colourVec4Data;
				colourVec4Parameters.push_back(materialBaseColourVec4Json);
			}
			json["colourVec4Parameters"] = colourVec4Parameters;
		}

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