#pragma once
#include <memory>
#include "EngineLayer.h"

namespace IcePick {
	class EngineAPI {
	public:
		EngineAPI() = delete;
		EngineAPI(std::shared_ptr<EngineLayer> engine);
		EngineAPI(const EngineAPI& other) = default;
		~EngineAPI();

		unsigned int GetTexture(std::filesystem::path assetPath);
		unsigned int GetTextureRenderId(UUID textureId);
		UUID LoadTextureFromAsset(std::filesystem::path assetPath);

		UUID CreateShaderFromSource(ShaderSource& source);
		ShaderProgram& GetShaderProgram(UUID shaderId);
		void UpdateShaderWithSource(UUID shaderId, ShaderSource& source);
		std::string LoadShaderSourceFile(std::filesystem::path filepath);

		const Texture& GetTexture(UUID textureId);

		void UpdateMaterialAsset(UUID materialId, MaterialAsset& material);

		MeshRendererComponent LoadMesh(std::filesystem::path assetPath);
		void RenderMesh(MeshRendererComponent& mesh, glm::mat4& modelMatrix, const entt::entity entityId);

		void SetRenderTargetDefault();
		void SetRenderTargetFrameBuffer();
		void ReloadShaders();

		void GetRendererWindowSize(int& x, int& y);
		glm::vec2 GetMousePos();
		glm::vec2 GetMouseDelta();
		void GetEntityMatPixelData(int x, int y, void* pixelData);
	private:
		std::shared_ptr<EngineLayer> m_Engine;
	};
}