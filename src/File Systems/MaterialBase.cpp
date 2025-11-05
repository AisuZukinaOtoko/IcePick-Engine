#include "MaterialBase.h"
#include "ShaderLoader.h"
#include "../Layers/EngineAPI.h"
#include "../LogSystem.h"

void IcePick::MaterialBase::ClearShaderInputs() {
	m_ShaderInputFlags = 0;
}

void IcePick::MaterialBase::AddShaderInput(ShaderInput inputType) {
	m_ShaderInputFlags |= inputType;
}

void IcePick::MaterialBase::ClearMaterialBaseData() {
	MaterialTextures.clear();
}

void IcePick::MaterialBase::BindMaterialInstanceTextures(EngineAPI engineAPI, const MaterialInstance& materialInstance) {
    if (Id != materialInstance.MaterialBaseId) {
        //IP_LOG("Material Base and Instance mismatch.", IP_WARN_LOG);
        //return;
    }

    IcePick::ShaderProgram& materialShader = engineAPI.GetShaderProgram(ShaderId);

    for (int i = 0; i < MaterialTextures.size(); i++) {
        std::string& textureSampler = MaterialTextures[i].SamplerIdentifier;
        IcePick::UUID textureId = materialInstance.GetMaterialInstanceTextureId(MaterialTextures[i].Id);

        const Texture& materialTexture = engineAPI.GetTexture(textureId);
        materialTexture.Bind(i);
        materialShader.SetUniformInt32(textureSampler.c_str(), i);
    }
}

void IcePick::MaterialInstance::SetMaterialInstanceTextureId(UUID materialBaseDataId, UUID textureId) {
    for (auto& textureData : InstanceTextureData) {
        if (textureData.MaterialBaseDataId == materialBaseDataId)
            textureData.Data = textureId;
    }
}

void IcePick::MaterialInstance::ClearMaterialInstanceData() {
	InstanceTextureData.clear();
}

IcePick::UUID IcePick::MaterialInstance::GetMaterialInstanceTextureId(UUID materialBaseDataId) const {
    for (const auto& textureData : InstanceTextureData) {
        if (textureData.MaterialBaseDataId == materialBaseDataId)
            return textureData.Data;
    }

    return UUID::Unitialised();
}
