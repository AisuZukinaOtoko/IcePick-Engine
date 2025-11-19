#pragma once
#include <vector>
#include <string>
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

		void ClearMaterialInstanceData();

		UUID GetMaterialInstanceTextureId(UUID materialBaseDataId) const;
		void SetMaterialInstanceTextureId(UUID materialBaseDataId, UUID textureId);
	private:
	};


	struct MaterialBaseTextureData {
		UUID Id;
		std::string SamplerIdentifier;
		//std::string DisplayName;
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

		void ClearShaderInputs();
		void AddShaderInput(ShaderInput inputType);
		void ClearMaterialBaseData();

		void BindMaterialInstanceTextures(EngineAPI engineAPI, const MaterialInstance& materialInstance);
		MaterialInstance CreateEmptyInstanceFromBase() const;
	private:
		unsigned int m_ShaderInputFlags = 0;
	};
}