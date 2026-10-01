#include "../Render Systems/Renderer.h"
#include "../Render Systems/VertexArray.h"
#include "../Render Systems/VertexLayout.h"
#include "TerrainSystem.h"
#include "../Public/EngineAPI.h"
#include "../File Systems/MaterialLoader.h"
#include "../File Systems/ShaderLoader.h"
#include "../File Systems/TextureLoader.h"
#include "../LogSystem.h"
#include <vector>

namespace IcePick {
	TerrainSystem::TerrainSystem() {

	}

	void TerrainSystem::Init(ShaderLoader& shaderLoader, MaterialLoader& materialLoader) {
		ShaderSource terrainShaderSource;
		terrainShaderSource.VertexShaderSource = shaderLoader.LoadFile("Game Engine/res/Shaders/terrain.vert.shader", 0);
		terrainShaderSource.FragmentShaderSource = shaderLoader.LoadFile("Game Engine/res/Shaders/terrain.frag.shader", 0);

		MaterialBase terrainMaterialBase;
		UUID baseTextureDataId;
		terrainMaterialBase.MaterialTextures.push_back({"", "u_AlbedoTexUnit", baseTextureDataId });
		terrainMaterialBase.ShaderId = shaderLoader.CreateShaderProgram(terrainShaderSource);
		MaterialInstance terrainMaterialInstance = terrainMaterialBase.CreateEmptyInstanceFromBase();

		UUID defaultMaterialBaseId = materialLoader.NewMaterialBaseFromCopy(terrainMaterialBase);
		m_DefaultTerrainMaterialInstanceId = materialLoader.NewMaterialInstanceFromCopy(terrainMaterialInstance);

		TerrainMaterialInstanceIds.push_back(m_DefaultTerrainMaterialInstanceId);
	} 

	void TerrainSystem::CreateTerrain(unsigned int resolution, TextureLoader& textureLoader) {
		if (resolution < 2) {
			IP_LOG("Cannot create terrain with a resolution less than 2 units.", IP_ERROR_LOG);
			return;
		}

		Destroy(textureLoader);

		const unsigned int indicesPerTriangle = 3;
		const unsigned int numberOfQuads = (resolution - 1) * (resolution - 1);
		const unsigned int numberOfIndices = numberOfQuads * 6;

		std::vector<IcePickRenderer::TerrainVertex3D> vertexBuffer(resolution * resolution);
		//std::vector<unsigned int> indexBuffer(numberOfIndices);
		std::vector<unsigned int> indexBuffer;

		for (unsigned int x = 0; x < resolution; x++) {
			for (unsigned int y = 0; y < resolution; y++) {
				glm::vec2 position{ x - resolution / 2.0f, y - resolution / 2.0f };
				glm::vec2 uv{ (x + 1) / (float)resolution, (y + 1) / (float)resolution };
				vertexBuffer[x * resolution + y] = { position, uv };
			}
		}

		for (unsigned int x = 0; x < resolution - 1; x++) {
			for (unsigned int y = 0; y < resolution - 1; y++) {
				const unsigned int topLeft = x + y * resolution;
				const unsigned int topRight = topLeft + 1;
				const unsigned int bottomLeft = (y + 1) * resolution + x;
				const unsigned int bottomRight = bottomLeft + 1;

				indexBuffer.emplace_back(bottomLeft);
				indexBuffer.emplace_back(topLeft);
				indexBuffer.emplace_back(topRight);

				indexBuffer.emplace_back(bottomLeft);
				indexBuffer.emplace_back(topRight);
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

		std::vector<float> heightMapData(resolution * resolution);
		IcePickRenderer::TextureSettings heightMapTextureSettings;
		m_HeightMapTextureId = textureLoader.NewUncompressedTextureFromMemory(heightMapData.data(), heightMapTextureSettings, false);

		m_Created = true;

	}

	void TerrainSystem::SetHeightMapTexture(UUID textureId) {
		m_HeightMapTextureId = textureId;
	}

	void TerrainSystem::RenderTerrain(MaterialLoader& materialLoader, ShaderLoader& shaderLoader, EngineAPI engineAPI) {
		if (!m_Created)
			return;

		for (const UUID& terrainMaterialInstanceId : TerrainMaterialInstanceIds) {
			MaterialInstance& terrainMaterialInstance = materialLoader.GetMaterialInstance(terrainMaterialInstanceId);
			MaterialBase& terrainMaterialBase = materialLoader.GetMaterialBase(terrainMaterialInstance.MaterialBaseId);
			ShaderProgram& terrainMaterialShaderProgram = shaderLoader.GetShaderProgram(terrainMaterialBase.ShaderId);

			terrainMaterialBase.BindMaterialInstanceParameters(engineAPI, terrainMaterialInstance);

			IcePickRenderer::DrawMeshNoUniforms(*m_InternalTerrainVertexArray, glm::mat4(1.0f), terrainMaterialShaderProgram);
		}
	}

	void TerrainSystem::Destroy(TextureLoader& textureLoader) {
		if (m_InternalTerrainVertexArray) {
			m_InternalTerrainVertexArray->Destroy();
			delete m_InternalTerrainVertexArray;
			m_InternalTerrainVertexArray = nullptr;
		}

		TerrainMaterialInstanceIds.clear();
		TerrainMaterialInstanceIds.push_back(m_DefaultTerrainMaterialInstanceId);

		textureLoader.DestroyTextureById(m_HeightMapTextureId);
		m_HeightMapTextureId = UUID::Unitialised();
		m_Created = false;
	}

	TerrainSystem::~TerrainSystem() {

	}
}