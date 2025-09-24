#pragma once
#include "../Render Systems/Renderer.h"
#include "Layer.h"
#include "../Render Systems/FrameBuffer.h"
#include "../Scene Systems/Scene.h"
#include "../Scene Systems/SceneRegistry.h"
#include "../File Systems/AssetLoader.h"

namespace IcePick {
	class EngineLayer : public Layer {
	public:
		void OnAttach() override;
		void OnUpdate(DeltaTime dt) override;
		void OnDetach() override;

		void OnEvent(Event& event) override;
		void OnNewFrame() override;
		void OnRender(RenderPayload& payload) override;

		unsigned int CreateTexture(std::filesystem::path assetPath);
		unsigned int GetTextureRenderId(UUID textureId);
		UUID LoadTextureFromAsset(std::filesystem::path assetPath);
		void SetRenderTargetDefault();
		void SetRenderTargetFrameBuffer();
		void ReloadShaders();

		void GetEntityMatPixelData(int x, int y, void* pixelData);
	private:
		FrameBuffer m_FrameBuffer;
		AssetLoader m_AssetLoader;
		void RenderEntityMeshes();
		void RenderMeshNode(const MeshNode& parent, glm::mat4 parentTransform, const std::vector<UUID>& materialSlots, const entt::entity entityId);
		int temp_DeleteLater = 0;
		Scene m_CurrentScene;
	};
}