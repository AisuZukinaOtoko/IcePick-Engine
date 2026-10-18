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
		void CreateTerrain(unsigned int size, unsigned int unitsPerMetre, TextureLoader& textureLoader); // resolution x resolution heightmap
		TerrainSystem(const TerrainSystem& other) = delete;

		void SetHeightMapTexture(UUID textureId);
		UUID GetHeightMapTexture();
		void RenderTerrain(MaterialLoader& materialLoader, ShaderLoader& shaderLoader, EngineAPI engineAPI);

		bool IsCreated() { return m_Created;  };

		void Destroy(TextureLoader& textureLoader);
		~TerrainSystem();

		std::vector<UUID> TerrainMaterialInstanceIds;
		float TerrainHeightScale = 20.0f;
	private:

		unsigned int m_TerrainSize = 2;
		unsigned int m_TerrainUnitsPerMetre = 1;

		bool m_Created = false;
		UUID m_HeightMapTextureId = UUID::Unitialised();
		UUID m_DefaultTerrainMaterialInstanceId = UUID::Unitialised();
		IcePickRenderer::VertexArray* m_InternalTerrainVertexArray = nullptr;
	};
}