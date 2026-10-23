#include "../Render Systems/Renderer.h"
#include "../Render Systems/Vertex.h"
#include "SceneUtils.h"

namespace IcePick {
	void GenerateTerrainVertexData(std::vector<IcePickRenderer::TerrainVertex3D>& vertices, std::vector<unsigned int>& indices, unsigned int size, unsigned int unitsPerMetre) {
		unsigned int resolution = size * unitsPerMetre;
		vertices.resize(resolution * resolution);
		indices.clear();

		float halfSize = size / 2.0f;
		for (unsigned int x = 0; x < resolution; x++) {
			for (unsigned int y = 0; y < resolution; y++) {
				glm::vec2 position{ (x / (float)(resolution - 1)) * size - halfSize, (y / (float)(resolution - 1)) * size - halfSize };
				glm::vec2 uv{ (x + 1) / (float)resolution, ((y + 1) / (float)resolution) };
				vertices[x * resolution + y] = { position, uv };
			}
		}

		for (unsigned int x = 0; x < resolution - 1; x++) {
			for (unsigned int y = 0; y < resolution - 1; y++) {
				const unsigned int topLeft = x + y * resolution;
				const unsigned int topRight = topLeft + 1;
				const unsigned int bottomLeft = (y + 1) * resolution + x;
				const unsigned int bottomRight = bottomLeft + 1;

				indices.emplace_back(bottomLeft);
				indices.emplace_back(topLeft);
				indices.emplace_back(topRight);
				
				indices.emplace_back(bottomLeft);
				indices.emplace_back(topRight);
				indices.emplace_back(bottomRight);
			}
		}
	}
}