#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "../Scene Systems/UUID.h"

namespace IcePick {
	class EngineAPI;
}

namespace IcePick {
	template<typename TData>
	struct MaterialInstanceData {
		MaterialInstanceData() = delete;
		MaterialInstanceData(UUID materialBaseDataId, TData data) : 
			MaterialBaseDataId(materialBaseDataId),
			Data(data) {}

		UUID Id;
		UUID MaterialBaseDataId = UUID::Unitialised();
		TData Data;
	};

	class MaterialInstance {
	public:
		MaterialInstance() = default;
		MaterialInstance(const MaterialInstance& other);
		UUID Id;
		UUID MaterialBaseId = UUID::Unitialised();
		std::vector<MaterialInstanceData<UUID>> InstanceTextureData;
		std::vector<MaterialInstanceData<float>> InstanceFloatData;
		std::vector<MaterialInstanceData<glm::vec2>> InstanceVec2Data;
		std::vector<MaterialInstanceData<glm::vec3>> InstanceVec3Data;
		std::vector<MaterialInstanceData<glm::vec4>> InstanceVec4Data;
		std::vector<MaterialInstanceData<glm::vec4>> InstanceColourVec4Data;

		void ClearMaterialInstanceData();

		UUID GetMaterialInstanceTextureId(UUID materialBaseDataId) const;
		void SetMaterialInstanceTextureId(UUID materialBaseDataId, UUID textureId);

		float GetMaterialInstanceFloatParameter(UUID materialBaseDataId) const;
		glm::vec2 GetMaterialInstanceVec2Parameter(UUID materialBaseDataId) const;
		glm::vec3 GetMaterialInstanceVec3Parameter(UUID materialBaseDataId) const;
		glm::vec4 GetMaterialInstanceVec4Parameter(UUID materialBaseDataId) const;
		glm::vec4 GetMaterialInstanceColourVec4Parameter(UUID materialBaseDataId) const;
	private:
	};


	struct MaterialBaseTextureData {
		std::string DisplayName;
		std::string SamplerIdentifier;
		UUID Id;
	};

	struct MaterialBaseParameter {
		std::string DisplayName;
		std::string ShaderIdentifier;
		UUID Id;
	};

	struct MaterialBaseReadRenderTexture {
		enum class RenderTextureType {
			COLOUR = 0,
			ACTIVE_COLOUR,
			DEPTH_STENCIL
		} TextureType{ RenderTextureType::COLOUR };
	};

	class MaterialBase {
	public:
		enum ShaderInput {
			DELTA_TIME = 0b1 << 0
		};

		UUID Id;
		UUID ShaderId = UUID::Unitialised();
		UUID ShaderGraphId = UUID::Unitialised();
		std::vector<MaterialBaseTextureData> MaterialTextures;
		std::vector<MaterialBaseParameter> MaterialFloatParameters;
		std::vector<MaterialBaseParameter> MaterialVec2Parameters;
		std::vector<MaterialBaseParameter> MaterialVec3Parameters;
		std::vector<MaterialBaseParameter> MaterialVec4Parameters;
		std::vector<MaterialBaseParameter> MaterialColourVec4Parameters;
		std::vector<MaterialBaseReadRenderTexture> MaterialReadRenderTextures;

		bool WriteDepthTexture = true;

		void ClearShaderInputs();
		void AddShaderInput(ShaderInput inputType);
		void ClearMaterialBaseData();

		void BindMaterialInstanceParameters(EngineAPI engineAPI, const MaterialInstance& materialInstance);
		MaterialInstance CreateEmptyInstanceFromBase() const;
	private:
		unsigned int m_ShaderInputFlags = 0;
	};
}