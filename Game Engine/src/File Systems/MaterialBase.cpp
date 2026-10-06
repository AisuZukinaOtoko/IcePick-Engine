#include "MaterialBase.h"
#include "ShaderLoader.h"
#include "../Public/EngineAPI.h"
#include "../LogSystem.h"

namespace IcePick {


    void MaterialBase::ClearMaterialBaseData() {
	    MaterialTextures.clear();
        MaterialFloatParameters.clear();
        MaterialVec2Parameters.clear();
        MaterialVec3Parameters.clear();
        MaterialVec4Parameters.clear();
        MaterialColourVec4Parameters.clear();
    }

    void MaterialBase::BindMaterialInstanceParameters(EngineAPI engineAPI, const MaterialInstance& materialInstance) {
        ShaderProgram& materialShader = engineAPI.GetShaderProgram(ShaderId);

        for (int i = 0; i < MaterialTextures.size(); i++) {
            std::string& textureSampler = MaterialTextures[i].SamplerIdentifier;
            UUID textureId = materialInstance.GetMaterialInstanceTextureId(MaterialTextures[i].Id);

            const IcePickRenderer::Texture& materialTexture = engineAPI.GetTexture(textureId);
            materialTexture.Bind(i);
            materialShader.SetUniformInt32(textureSampler.c_str(), i);
        }

        for (int i = 0; i < MaterialFloatParameters.size(); i++) {
            std::string& floatUniform = MaterialFloatParameters[i].ShaderIdentifier;
            float instanceFloatDataValue = materialInstance.GetMaterialInstanceFloatParameter(MaterialFloatParameters[i].Id);
            materialShader.SetUniformFloat(floatUniform.c_str(), instanceFloatDataValue);
        }

        for (int i = 0; i < MaterialVec2Parameters.size(); i++) {
            std::string& uniform = MaterialVec2Parameters[i].ShaderIdentifier;
            glm::vec2 instanceVec2DataValue = materialInstance.GetMaterialInstanceVec2Parameter(MaterialVec2Parameters[i].Id);
            materialShader.SetUniformVec2(uniform.c_str(), instanceVec2DataValue);
        }

        for (int i = 0; i < MaterialVec3Parameters.size(); i++) {
            std::string& uniform = MaterialVec3Parameters[i].ShaderIdentifier;
            glm::vec3 instanceVec3DataValue = materialInstance.GetMaterialInstanceVec3Parameter(MaterialVec3Parameters[i].Id);
            materialShader.SetUniformVec3(uniform.c_str(), instanceVec3DataValue);
        }

        for (int i = 0; i < MaterialVec4Parameters.size(); i++) {
            std::string& uniform = MaterialVec4Parameters[i].ShaderIdentifier;
            glm::vec4 instanceVec4DataValue = materialInstance.GetMaterialInstanceVec4Parameter(MaterialVec4Parameters[i].Id);
            materialShader.SetUniformVec4(uniform.c_str(), instanceVec4DataValue);
        }

        for (int i = 0; i < MaterialColourVec4Parameters.size(); i++) {
            std::string& uniform = MaterialColourVec4Parameters[i].ShaderIdentifier;
            glm::vec4 instanceColourVec4DataValue = materialInstance.GetMaterialInstanceColourVec4Parameter(MaterialColourVec4Parameters[i].Id);
            materialShader.SetUniformVec4(uniform.c_str(), instanceColourVec4DataValue);
        }
    }

    UUID MaterialBase::GetFloatParameterBaseIdByDisplayName(const std::string& displayName) const {
        for (const auto& parameter : MaterialFloatParameters) {
            if (parameter.DisplayName == displayName)
                return parameter.Id;
        }
        return UUID::Unitialised();
    }

    UUID MaterialBase::GetVec2ParameterBaseIdByDisplayName(const std::string& displayName) const {
        for (const auto& parameter : MaterialVec2Parameters) {
            if (parameter.DisplayName == displayName)
                return parameter.Id;
        }
        return UUID::Unitialised();
    }

    UUID MaterialBase::GetVec3ParameterBaseIdByDisplayName(const std::string& displayName) const {
        for (const auto& parameter : MaterialVec3Parameters) {
            if (parameter.DisplayName == displayName)
                return parameter.Id;
        }
        return UUID::Unitialised();
    }

    UUID MaterialBase::GetVec4ParameterBaseIdByDisplayName(const std::string& displayName) const {
        for (const auto& parameter : MaterialVec4Parameters) {
            if (parameter.DisplayName == displayName)
                return parameter.Id;
        }
        return UUID::Unitialised();
    }

    UUID MaterialBase::GetColourVec4ParameterBaseIdByDisplayName(const std::string& displayName) const {
        for (const auto& parameter : MaterialColourVec4Parameters) {
            if (parameter.DisplayName == displayName)
                return parameter.Id;
        }
        return UUID::Unitialised();
    }


    MaterialInstance::MaterialInstance(const MaterialInstance& other) {
        Id = other.Id;
        MaterialBaseId = other.MaterialBaseId;
        InstanceTextureData = other.InstanceTextureData;
        InstanceFloatData = other.InstanceFloatData;
        InstanceVec2Data = other.InstanceVec2Data;
        InstanceVec3Data = other.InstanceVec3Data;
        InstanceVec4Data = other.InstanceVec4Data;
        InstanceColourVec4Data = other.InstanceColourVec4Data;
    }

    MaterialInstance MaterialBase::CreateEmptyInstanceFromBase() const {
        MaterialInstance tempMaterialInstance;
        tempMaterialInstance.MaterialBaseId = Id;

        for (const auto& textureParameter : MaterialTextures) {
            tempMaterialInstance.InstanceTextureData.emplace_back(textureParameter.Id, UUID::Unitialised());
        }

        for (const auto& floatParameter : MaterialFloatParameters) {
            tempMaterialInstance.InstanceFloatData.emplace_back(floatParameter.Id, 0.0f);
        }

        for (const auto& vec2Parameter : MaterialVec2Parameters) {
            tempMaterialInstance.InstanceVec2Data.emplace_back(vec2Parameter.Id, glm::vec2(0.0f, 0.0f));
        }

        for (const auto& vec3Parameter : MaterialVec3Parameters) {
            tempMaterialInstance.InstanceVec3Data.emplace_back(vec3Parameter.Id, glm::vec3(0.0f, 0.0f, 0.0f));
        }

        for (const auto& vec4Parameter : MaterialVec4Parameters) {
            tempMaterialInstance.InstanceVec4Data.emplace_back(vec4Parameter.Id, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
        }

        for (const auto& colourVec4Parameter : MaterialColourVec4Parameters) {
            tempMaterialInstance.InstanceColourVec4Data.emplace_back(colourVec4Parameter.Id, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        }

        return tempMaterialInstance;
    }

    void MaterialInstance::SetMaterialInstanceTextureId(UUID materialBaseDataId, UUID textureId) {
        for (auto& textureData : InstanceTextureData) {
            if (textureData.MaterialBaseDataId == materialBaseDataId)
                textureData.Data = textureId;
        }
    }

    void MaterialInstance::ClearMaterialInstanceData() {
	    InstanceTextureData.clear();
        InstanceFloatData.clear();
        InstanceVec2Data.clear();
        InstanceVec3Data.clear();
        InstanceVec4Data.clear();
        InstanceColourVec4Data.clear();
    }

    UUID MaterialInstance::GetMaterialInstanceTextureId(UUID materialBaseDataId) const {
        for (const auto& textureData : InstanceTextureData) {
            if (textureData.MaterialBaseDataId == materialBaseDataId)
                return textureData.Data;
        }

        return UUID::Unitialised();
    }

    float MaterialInstance::GetMaterialInstanceFloatParameter(UUID materialBaseDataId) const {
        for (const auto& floatData : InstanceFloatData) {
            if (floatData.MaterialBaseDataId == materialBaseDataId)
                return floatData.Data;
        }

        return 0.0f;
    }

    glm::vec2 MaterialInstance::GetMaterialInstanceVec2Parameter(UUID materialBaseDataId) const {
        for (const auto& vec2Data : InstanceVec2Data) {
            if (vec2Data.MaterialBaseDataId == materialBaseDataId)
                return vec2Data.Data;
        }

        return glm::vec2(0.0f);
    }


    glm::vec3 MaterialInstance::GetMaterialInstanceVec3Parameter(UUID materialBaseDataId) const {
        for (const auto& vec3Data : InstanceVec3Data) {
            if (vec3Data.MaterialBaseDataId == materialBaseDataId)
                return vec3Data.Data;
        }

        return glm::vec3(0.0f);
    }

    glm::vec4 MaterialInstance::GetMaterialInstanceVec4Parameter(UUID materialBaseDataId) const {
        for (const auto& vec4Data : InstanceVec4Data) {
            if (vec4Data.MaterialBaseDataId == materialBaseDataId)
                return vec4Data.Data;
        }

        return glm::vec4(0.0f);
    }

    glm::vec4 MaterialInstance::GetMaterialInstanceColourVec4Parameter(UUID materialBaseDataId, bool convertToNonLinear) const {
        for (const auto& colourVec4Data : InstanceColourVec4Data) {
            if (colourVec4Data.MaterialBaseDataId == materialBaseDataId) {
                // TODO: Convert to non-linear colour space
                return colourVec4Data.Data;
            }            
        }

        return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    void MaterialInstance::SetMaterialInstanceFloatParameter(UUID materialBaseDataId, float value) {
        for (auto& floatData : InstanceFloatData) {
            if (floatData.MaterialBaseDataId == materialBaseDataId)
                floatData.Data = value;
        }
    }

    void MaterialInstance::SetMaterialInstanceVec2Parameter(UUID materialBaseDataId, glm::vec2 value) {
        for (auto& vec2Data : InstanceVec2Data) {
            if (vec2Data.MaterialBaseDataId == materialBaseDataId)
                vec2Data.Data = value;
        }
    }

    void MaterialInstance::SetMaterialInstanceVec3Parameter(UUID materialBaseDataId, glm::vec3 value) {
        for (auto& vec3Data : InstanceVec3Data) {
            if (vec3Data.MaterialBaseDataId == materialBaseDataId)
                vec3Data.Data = value;
        }
    }

    void MaterialInstance::SetMaterialInstanceVec4Parameter(UUID materialBaseDataId, glm::vec4 value) {
        for (auto& vec4Data : InstanceVec4Data) {
            if (vec4Data.MaterialBaseDataId == materialBaseDataId)
                vec4Data.Data = value;
        }
    }

    void MaterialInstance::SetMaterialInstanceColourVec4Parameter(UUID materialBaseDataId, glm::vec4 value) {
        for (auto& colourVec4Data : InstanceColourVec4Data) {
            if (colourVec4Data.MaterialBaseDataId == materialBaseDataId)
                colourVec4Data.Data = value;
            
        }
    }

}