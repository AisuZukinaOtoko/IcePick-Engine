#include "MaterialBase.h"
#include "ShaderLoader.h"
#include "../Public/EngineAPI.h"
#include "../LogSystem.h"

void IcePick::MaterialBase::ClearShaderInputs() {
	m_ShaderInputFlags = 0;
}

void IcePick::MaterialBase::AddShaderInput(ShaderInput inputType) {
	m_ShaderInputFlags |= inputType;
}

void IcePick::MaterialBase::ClearMaterialBaseData() {
	MaterialTextures.clear();
    MaterialFloatParameters.clear();
}

void IcePick::MaterialBase::BindMaterialInstanceParameters(EngineAPI engineAPI, const MaterialInstance& materialInstance) {
    IcePick::ShaderProgram& materialShader = engineAPI.GetShaderProgram(ShaderId);

    for (int i = 0; i < MaterialTextures.size(); i++) {
        std::string& textureSampler = MaterialTextures[i].SamplerIdentifier;
        IcePick::UUID textureId = materialInstance.GetMaterialInstanceTextureId(MaterialTextures[i].Id);

        const IcePickRenderer::Texture& materialTexture = engineAPI.GetTexture(textureId);
        materialTexture.Bind(i);
        materialShader.SetUniformInt32(textureSampler.c_str(), i);
    }

    for (int i = 0; i < MaterialFloatParameters.size(); i++) {
        std::string& floatUniform = MaterialFloatParameters[i].ShaderIdentifier;
        float instanceFloatDataValue = materialInstance.GetMaterialInstanceFloatParameter(MaterialFloatParameters[i].Id);
        materialShader.SetUniformFloat(floatUniform.c_str(), instanceFloatDataValue);
    }
}

IcePick::MaterialInstance::MaterialInstance(const MaterialInstance& other) {
    Id = other.Id;
    MaterialBaseId = other.MaterialBaseId;
    InstanceTextureData = other.InstanceTextureData;
    InstanceFloatData = other.InstanceFloatData;
}

IcePick::MaterialInstance IcePick::MaterialBase::CreateEmptyInstanceFromBase() const {
    MaterialInstance tempMaterialInstance;
    tempMaterialInstance.MaterialBaseId = Id;

    for (const auto& textureParameter : MaterialTextures) {
        tempMaterialInstance.InstanceTextureData.emplace_back(textureParameter.Id, UUID::Unitialised());
    }

    for (const auto& floatParameter : MaterialFloatParameters) {
        tempMaterialInstance.InstanceFloatData.emplace_back(floatParameter.Id, 0.0f);
    }

    return tempMaterialInstance;
}

void IcePick::MaterialInstance::SetMaterialInstanceTextureId(UUID materialBaseDataId, UUID textureId) {
    for (auto& textureData : InstanceTextureData) {
        if (textureData.MaterialBaseDataId == materialBaseDataId)
            textureData.Data = textureId;
    }
}

void IcePick::MaterialInstance::ClearMaterialInstanceData() {
	InstanceTextureData.clear();
    InstanceFloatData.clear();
}

IcePick::UUID IcePick::MaterialInstance::GetMaterialInstanceTextureId(UUID materialBaseDataId) const {
    for (const auto& textureData : InstanceTextureData) {
        if (textureData.MaterialBaseDataId == materialBaseDataId)
            return textureData.Data;
    }

    return UUID::Unitialised();
}

float IcePick::MaterialInstance::GetMaterialInstanceFloatParameter(UUID materialBaseDataId) const {
    for (const auto& floatData : InstanceFloatData) {
        if (floatData.MaterialBaseDataId == materialBaseDataId)
            return floatData.Data;
    }

    return 0.0f;
}
