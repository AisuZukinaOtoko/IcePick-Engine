#pragma once
#include "UUID.h"
#include "TerrainSettings.h"
#include <vector>

namespace IcePickRenderer {
	class VertexArray;
}

namespace IcePick {
	class ShaderLoader;
	class MaterialLoader;
	class TextureLoader;
	class PhysicsSystem3D;
	class EngineAPI;

	class TerrainSystem {
	public:
		TerrainSystem();
		void Init(ShaderLoader& shaderLoader, MaterialLoader& materialLoader);
		void InitTerrainPhysics(PhysicsSystem3D& physicsSystem);
		void CreateTerrain(unsigned int size, unsigned int unitsPerMetre, TextureLoader& textureLoader); // resolution x resolution heightmap
		TerrainSystem(const TerrainSystem& other) = delete;

		void SetHeightMapTexture(UUID textureId);
		UUID GetHeightMapTexture() const;
		void RenderTerrain(MaterialLoader& materialLoader, ShaderLoader& shaderLoader, EngineAPI engineAPI);

		bool IsCreated() const { return m_Created;  };

		void Destroy(TextureLoader& textureLoader);
		~TerrainSystem();

		TerrainSettings TerrainSettings;
		std::vector<UUID> TerrainMaterialInstanceIds;
	private:
		bool m_Created = false;
		UUID m_HeightMapTextureId = UUID::Unitialised();
		UUID m_DefaultTerrainMaterialInstanceId = UUID::Unitialised();
		IcePickRenderer::VertexArray* m_InternalTerrainVertexArray = nullptr;
	};
}