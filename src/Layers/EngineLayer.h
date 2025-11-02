#pragma once
#include "../Render Systems/Renderer.h"
#include "Layer.h"
#include "../Render Systems/FrameBuffer.h"
#include "../Scene Systems/Scene.h"
#include "../Scene Systems/SceneRegistry.h"
#include "../File Systems/AssetLoader.h"



namespace IcePick {
	class EngineAPI;

	class EngineLayer : public Layer, public std::enable_shared_from_this<EngineLayer> {
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

		//void InitThumbnailBuffer
		void GetEntityMatPixelData(int x, int y, void* pixelData);
	private:
		friend class EngineAPI;
		FrameBuffer m_FrameBuffer;
		FrameBuffer m_ThumbnailBuffer;
		AssetLoader m_AssetLoader;
		void RenderEntityMeshes();
		void RenderMeshNode(const MeshNode& parent, glm::mat4 parentTransform, const std::vector<UUID>& materialSlots, const entt::entity entityId);
		int temp_DeleteLater = 0;
		Scene m_CurrentScene;
	};
}