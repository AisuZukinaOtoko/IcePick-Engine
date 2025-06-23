#pragma once
#include "../Render Systems/Renderer.h"
#include "Layer.h"
#include "../Render Systems/FrameBuffer.h"
#include "../Scene Systems/Scene.h"
#include "../File Systems/AssetLoader.h"

namespace IcePick {
	class EngineLayer : public Layer {
	public:
		void OnAttach() override;
		void OnUpdate(DeltaTime dt) override;
		void OnDetach() override;

		void OnEvent(Event& event) override;
		void OnRender(RenderPayload& payload) override;

		unsigned int CreateTexture(std::filesystem::path assetPath);
	private:
		FrameBuffer m_FrameBuffer;
		AssetLoader m_AssetLoader;
		void RenderEntityMeshes();
		int temp_DeleteLater = 0;
		Scene m_CurrentScene;
	};
}