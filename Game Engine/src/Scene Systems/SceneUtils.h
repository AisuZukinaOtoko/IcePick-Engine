#pragma once
#include <vector>

namespace IcePickRenderer {
	struct TerrainVertex3D;
}

namespace IcePick {
	void GenerateTerrainVertexData(std::vector<IcePickRenderer::TerrainVertex3D>& vertices, std::vector<unsigned int>& indices, unsigned int size, unsigned int unitsPerMetre);
}