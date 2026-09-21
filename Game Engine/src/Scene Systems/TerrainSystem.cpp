#include "../Render Systems/VertexArray.h"
#include "../Render Systems/VertexLayout.h"
#include "TerrainSystem.h"
#include "../LogSystem.h"
#include <vector>

namespace IcePick {
	TerrainSystem::TerrainSystem() {

	}

	void TerrainSystem::Init(unsigned int resolution) {
		if (resolution < 2) {
			IP_LOG("Cannot create terrain with a resolution less than 2 units.", IP_ERROR_LOG);
			return;
		}

		Destroy();

		const unsigned int indicesPerTriangle = 3;
		const unsigned int numberOfQuads = (resolution - 1) * (resolution - 1);
		const unsigned int numberOfIndices = numberOfQuads * 6;

		std::vector<IcePickRenderer::TerrainVertex3D> vertexBuffer(resolution * resolution);
		std::vector<unsigned int> indexBuffer(numberOfIndices);

		for (unsigned int x = 0; x < resolution; x++) {
			for (unsigned int y = 0; y < resolution; y++) {\
				glm::vec2 position{ x - resolution / 2.0f, y - resolution / 2.0f };
				glm::vec2 uv{ (x + 1) / (float)resolution, (y + 1) / (float)resolution };
				vertexBuffer.emplace_back(position, uv);
			}
		}

		for (unsigned int i = 0; i < numberOfQuads; i++) {
			indexBuffer.emplace_back(i);
		}

		for (unsigned int x = 0; x < resolution - 1; x++) {
			for (unsigned int y = 0; y < resolution - 1; y++) {
				const unsigned int topLeft = x + y * resolution;
				const unsigned int topRight = topLeft + 1;
				const unsigned int bottomLeft = (y + 1) * resolution + x;
				const unsigned int bottomRight = bottomLeft + 1;

				indexBuffer.emplace_back(topLeft);
				indexBuffer.emplace_back(bottomLeft);
				indexBuffer.emplace_back(topRight);

				indexBuffer.emplace_back(topRight);
				indexBuffer.emplace_back(bottomLeft);
				indexBuffer.emplace_back(bottomRight);
			}
		}

		m_InternalTerrainVertexArray = new IcePickRenderer::VertexArray(
			vertexBuffer.data(),
			vertexBuffer.size() * sizeof(IcePickRenderer::TerrainVertex3D),
			indexBuffer.data(),
			indexBuffer.size(),
			IcePickRenderer::TerrainVertex3D::GetVertexLayout(),
			IcePickRenderer::VertexType::TERRAIN_VERTEX
		);
	}

	void TerrainSystem::SetHeightMapTexture(UUID textureId) {
		m_HeightMapTextureId = textureId;
	}

	void TerrainSystem::RenderTerrain() {

	}

	void TerrainSystem::Destroy() {
		if (m_InternalTerrainVertexArray) {
			m_InternalTerrainVertexArray->Destroy();
			delete m_InternalTerrainVertexArray;
			m_InternalTerrainVertexArray = nullptr;
		}
	}

	TerrainSystem::~TerrainSystem() {

	}
}