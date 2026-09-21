#pragma once
#include "UUID.h"

namespace IcePickRenderer {
	class VertexArray;
}

namespace IcePick {
	class TerrainSystem {
	public:
		TerrainSystem();
		void Init(unsigned int resolution); // resolution x resolution heightmap
		TerrainSystem(const TerrainSystem& other) = delete;

		void SetHeightScale(float scale);
		void SetHeightMapTexture(UUID textureId);
		void RenderTerrain();

		void Destroy();
		~TerrainSystem();
	private:
		UUID m_HeightMapTextureId = UUID::Unitialised();
		IcePickRenderer::VertexArray* m_InternalTerrainVertexArray = nullptr;
	};
}