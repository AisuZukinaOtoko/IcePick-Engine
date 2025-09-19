#include "MaterialLoader.h"
#include "../LogSystem.h"
#include "../Utilities/Assert.h"

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
		m_DefaultMaterial.SampleBitmask |= MaterialAsset::ALBEDO;
		m_CachedMaterial = m_DefaultMaterial;
	}

	void MaterialLoader::InvalidateCache() {
		m_CachedMaterialId = UUID::Unitialised();
		m_CachedMaterial = m_DefaultMaterial;
	}

	void MaterialLoader::ConstructMaterial(const MaterialAsset& mat, Material& result, TextureLoader& textureLoader, ShaderLoader& shaderLoader) const {
		// Retrieving Texture IDs is expensive, so the check is necessary.
		// We don't reset the mat.TextureID in case the user wants to re-activate(sample) it

		// Albedo map
		if (mat.SampleBitmask & MaterialAsset::ALBEDO)
			result.AlbedoMap = textureLoader.GetTexture(mat.AlbedoTexture).GetID();
		else
			result.AlbedoMap = textureLoader.GetDefaultTexture().GetID();

		// Normal map
		if (mat.SampleBitmask & MaterialAsset::NORMAL)
			result.NormalMap = textureLoader.GetTexture(mat.NormalTexture).GetID();
		else
			result.NormalMap = textureLoader.GetDefaultTexture().GetID();

		// Roughness map
		if (mat.SampleBitmask & MaterialAsset::ROUGHNESS)
			result.RoughnessMap = textureLoader.GetTexture(mat.RoughnessTexture).GetID();
		else
			result.RoughnessMap = textureLoader.GetDefaultTexture().GetID();

		// Metallic map
		if (mat.SampleBitmask & MaterialAsset::METTALIC)
			result.MetallicMap = textureLoader.GetTexture(mat.MetallicTexture).GetID();
		else
			result.MetallicMap = textureLoader.GetDefaultTexture().GetID();

		// Emissive map
		if (mat.SampleBitmask & MaterialAsset::EMISSIVE)
			result.EmissiveMap = textureLoader.GetTexture(mat.EmissiveTexture).GetID();
		else
			result.EmissiveMap = textureLoader.GetDefaultTexture().GetID();

		result.SampleBitmask = mat.SampleBitmask;
		ShaderProgram& materialShader = shaderLoader.GetShaderProgram(mat.ShaderID);
		result.ShaderID = materialShader.GetID();

		result.AlbedoColour = mat.AlbedoColour;
		result.SpecularColour = mat.SpecularColour;
		result.RoughnessValue = mat.RoughnessValue;
		result.MetalValue = mat.MetalValue;
		result.EmissiveValue = mat.EmissiveValue;
	}

	UUID MaterialLoader::RegisterMaterialAsset(const MaterialAsset& materialAsset) {
		UUID materialId;
		m_LoadedMaterialAssets.insert({ materialId, materialAsset });
		return materialId;
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

	MaterialAsset& MaterialLoader::GetMaterialAsset(UUID Id) {
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
	}

	void MaterialLoader::SetLoadMaterialShaderID(UUID materialShaderId)	{
		m_LoadMaterialShaderId = materialShaderId;
	}

	/*void MaterialLoader::SetDefaultMaterial(Material defaultMaterial) {
		m_DefaultMaterial = defaultMaterial;
		InvalidateCache();
	}*/

	void MaterialLoader::UpdateMaterial(const Material& other) {

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

	void MaterialLoader::GetSceneMaterialColours(MaterialAsset& materialAsset, aiMaterial* mat) {
		aiColor4D materialColour;
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
		
	}

	UUID MaterialLoader::NewMaterialFromScene(const aiScene* scene, unsigned int materialIndex, TextureLoader& textureLoader) {
		// Check if scene material has been loaded before.
		auto iterator = m_CachedSceneMaterials.find(materialIndex);
		if (iterator != m_CachedSceneMaterials.end())
			return iterator->second;

		const unsigned int numMaterials = scene->mNumMaterials;
		IP_ASSERT(materialIndex <= numMaterials, "Material index out of bounds.");

		aiMaterial* mat = scene->mMaterials[materialIndex];
		
		MaterialAsset newMaterial;
		newMaterial.ShaderID = m_LoadMaterialShaderId;
		newMaterial.AlbedoTexture = GetSceneMaterialTexture(scene, aiTextureType_DIFFUSE, mat, textureLoader);
		newMaterial.NormalTexture = GetSceneMaterialTexture(scene, aiTextureType_NORMALS, mat, textureLoader);
		newMaterial.RoughnessTexture = GetSceneMaterialTexture(scene, aiTextureType_SHININESS, mat, textureLoader);
		newMaterial.MetallicTexture = GetSceneMaterialTexture(scene, aiTextureType_METALNESS, mat, textureLoader);
		newMaterial.EmissiveTexture = GetSceneMaterialTexture(scene, aiTextureType_EMISSIVE, mat, textureLoader);

		GetSceneMaterialColours(newMaterial, mat);
		SetMaterialSampleBits(newMaterial, UUID::Unitialised());

		UUID newMaterialId = RegisterMaterialAsset(newMaterial);
		m_CachedSceneMaterials.insert({ materialIndex, newMaterialId });
		return newMaterialId;
	}

	// Only called when loading a new material
	// Unitialised material texture slots will not be sampled within the shader
	void MaterialLoader::SetMaterialSampleBits(MaterialAsset& materialAsset, UUID defualtTextureId) {
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
	}

	void MaterialLoader::CleanUpAfterLoad()	{
		m_CachedSceneMaterials.clear();
	}

	void MaterialLoader::ShutDown(TextureLoader& textureLoader) {

	}

	MaterialLoader::~MaterialLoader() {

	}
}