#pragma once
#include "UUID.h"
#include <vector>

namespace IcePickRenderer {
	class VertexArray;
}

namespace IcePick {
	class ShaderLoader;
	class MaterialLoader;
	class TextureLoader;
	class EngineAPI;

	class TerrainSystem {
	public:
		TerrainSystem();
		void Init(ShaderLoader& shaderLoader, MaterialLoader& materialLoader);
		void CreateTerrain(unsigned int resolution, TextureLoader& textureLoader); // resolution x resolution heightmap
		TerrainSystem(const TerrainSystem& other) = delete;

		void SetHeightScale(float scale);
		void SetHeightMapTexture(UUID textureId);
		void RenderTerrain(MaterialLoader& materialLoader, ShaderLoader& shaderLoader, EngineAPI engineAPI);

		bool IsCreated() { return m_Created;  };

		void Destroy(TextureLoader& textureLoader);
		~TerrainSystem();

		std::vector<UUID> TerrainMaterialInstanceIds;
	private:
		bool m_Created = false;
		UUID m_HeightMapTextureId = UUID::Unitialised();
		UUID m_DefaultTerrainMaterialInstanceId = UUID::Unitialised();
		IcePickRenderer::VertexArray* m_InternalTerrainVertexArray = nullptr;
	};
}