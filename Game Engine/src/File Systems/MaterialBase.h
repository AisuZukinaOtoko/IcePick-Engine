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
		//std::vector<MaterialInstanceData<glm::vec4>> InstanceVec4Data;
		std::vector<MaterialInstanceData<float>> InstanceFloatData;

		void ClearMaterialInstanceData();

		UUID GetMaterialInstanceTextureId(UUID materialBaseDataId) const;
		void SetMaterialInstanceTextureId(UUID materialBaseDataId, UUID textureId);
		float GetMaterialInstanceFloatParameter(UUID materialBaseDataId) const;
	private:
	};


	struct MaterialBaseTextureData {
		std::string DisplayName;
		std::string SamplerIdentifier;
		UUID Id;
	};

	/*struct MaterialBaseVec4Parameter {
		std::string DisplayName;
		glm::vec4 Vec4Data;
		UUID Id;
	};*/

	struct MaterialBaseFloatParameter {
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
		//std::vector<MaterialBaseVec4Parameter> MaterialVec4Parameters;
		std::vector<MaterialBaseFloatParameter> MaterialFloatParameters;
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