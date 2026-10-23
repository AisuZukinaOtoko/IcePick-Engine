#include "../Render Systems/Renderer.h"
#include "../Render Systems/VertexArray.h"
#include "../Render Systems/VertexLayout.h"
#include "TerrainSystem.h"
#include "SceneUtils.h"
#include "../Physics Systems/PhysicsSystem3D.h"
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
		terrainMaterialBase.SurfaceType = MaterialBase::MaterialSurfaceType::TERRAIN_MESH;
		terrainMaterialBase.MaterialTextures.push_back({"Albedo Texture", "u_AlbedoTexUnit", baseTextureDataId });
		//terrainMaterialBase.MaterialTextures.push_back({ "Height Map", "u_HeightFieldTextureUnit", UUID{} });

		terrainMaterialBase.ShaderId = shaderLoader.CreateShaderProgram(terrainShaderSource);
		MaterialInstance terrainMaterialInstance = terrainMaterialBase.CreateEmptyInstanceFromBase();

		UUID defaultMaterialBaseId = materialLoader.NewMaterialBaseFromCopy(terrainMaterialBase);
		m_DefaultTerrainMaterialInstanceId = materialLoader.NewMaterialInstanceFromCopy(terrainMaterialInstance);

		TerrainMaterialInstanceIds.push_back(m_DefaultTerrainMaterialInstanceId);
	}

	void TerrainSystem::InitTerrainPhysics(PhysicsSystem3D& physicsSystem) {

	}


	void TerrainSystem::CreateTerrain(unsigned int size, unsigned int unitsPerMetre, TextureLoader& textureLoader) {
		if (size < 2) {
			IP_LOG("Cannot create terrain with a size less than 2 metres.", IP_ERROR_LOG);
			return;
		}
		
		if (unitsPerMetre < 1) {
			IP_LOG("Cannot create terrain with invalid units per metre.", IP_ERROR_LOG);
			return;
		}

		Destroy(textureLoader);

		unsigned int resolution = size * unitsPerMetre;
		std::vector<IcePickRenderer::TerrainVertex3D> vertexBuffer(resolution * resolution);
		std::vector<unsigned int> indexBuffer;

		GenerateTerrainVertexData(vertexBuffer, indexBuffer, size, unitsPerMetre);

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
		TerrainSettings.TerrainSize = size;
		TerrainSettings.TerrainUnitsPerMetre = unitsPerMetre;
	}

	void TerrainSystem::SetHeightMapTexture(UUID textureId) {
		m_HeightMapTextureId = textureId;
	}

	UUID TerrainSystem::GetHeightMapTexture() const {
		return m_HeightMapTextureId;
	}

	void TerrainSystem::RenderTerrain(MaterialLoader& materialLoader, ShaderLoader& shaderLoader, EngineAPI engineAPI) {
		if (!m_Created)
			return;

		for (const UUID& terrainMaterialInstanceId : TerrainMaterialInstanceIds) {
			MaterialInstance& terrainMaterialInstance = materialLoader.GetMaterialInstance(terrainMaterialInstanceId);
			MaterialBase& terrainMaterialBase = materialLoader.GetMaterialBase(terrainMaterialInstance.MaterialBaseId);
			ShaderProgram& terrainMaterialShaderProgram = shaderLoader.GetShaderProgram(terrainMaterialBase.ShaderId);
			terrainMaterialShaderProgram.SetUniformFloat("u_HeightScale", TerrainSettings.TerrainHeightScale);
			terrainMaterialShaderProgram.SetUniformFloat("u_TexelSize", 1.0f / (TerrainSettings.TerrainSize * TerrainSettings.TerrainUnitsPerMetre));

			const IcePickRenderer::Texture& materialTexture = engineAPI.GetTexture(m_HeightMapTextureId);
			materialTexture.Bind(terrainMaterialBase.MaterialTextures.size());
			terrainMaterialShaderProgram.SetUniformInt32("u_HeightFieldTextureUnit", terrainMaterialBase.MaterialTextures.size());

			terrainMaterialBase.BindMaterialInstanceParameters(engineAPI, terrainMaterialInstance);

			IcePickRenderer::DrawMesh(*m_InternalTerrainVertexArray, terrainMaterialShaderProgram);
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

		m_HeightMapTextureId = UUID::Unitialised();
		m_Created = false;
	}

	TerrainSystem::~TerrainSystem() {

	}
}