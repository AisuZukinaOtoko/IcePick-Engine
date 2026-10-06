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
		glm::vec4 GetMaterialInstanceColourVec4Parameter(UUID materialBaseDataId, bool convertToNonLinear = false) const;

		void SetMaterialInstanceFloatParameter(UUID materialBaseDataId, float value);
		void SetMaterialInstanceVec2Parameter(UUID materialBaseDataId, glm::vec2 value);
		void SetMaterialInstanceVec3Parameter(UUID materialBaseDataId, glm::vec3 value);
		void SetMaterialInstanceVec4Parameter(UUID materialBaseDataId, glm::vec4 value);
		void SetMaterialInstanceColourVec4Parameter(UUID materialBaseDataId, glm::vec4 value);
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
		
		enum MaterialSurfaceType {
			STATIC_MESH = 0,
			SKELETAL_MESH,
			MATERIAL_MESH_TYPE_COUNT
		} SurfaceType = STATIC_MESH;

		enum MaterialShadingModel {
			UNLIT = 0,
			PHONG_SHADING,
			MATERIAL_SHADING_MODEL_COUNT
		} ShadingModel = PHONG_SHADING;

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

		void ClearMaterialBaseData();

		UUID GetFloatParameterBaseIdByDisplayName(const std::string& displayName) const;
		UUID GetVec2ParameterBaseIdByDisplayName(const std::string& displayName) const;
		UUID GetVec3ParameterBaseIdByDisplayName(const std::string& displayName) const;
		UUID GetVec4ParameterBaseIdByDisplayName(const std::string& displayName) const;
		UUID GetColourVec4ParameterBaseIdByDisplayName(const std::string& displayName) const;

		void BindMaterialInstanceParameters(EngineAPI engineAPI, const MaterialInstance& materialInstance);
		MaterialInstance CreateEmptyInstanceFromBase() const;
	private:
	};
}