#pragma once
#include "Layer.h"
#include "../Event Systems/Input.h"
#include "../Render Systems/FrameBuffer.h"
#include "../Scene Systems/Scene.h"
#include "../Scene Systems/SceneRegistry.h"
#include "../File Systems/AssetLoader.h"
#include "../Scene Systems/ScriptRunner.h"
#include "EngineRuntimeStates.h"

#include "../Physics Systems/PhysicsSystem3D.h"


namespace IcePick {
	class EngineAPI;

	class EngineLayer : public Layer, public std::enable_shared_from_this<EngineLayer> {
	public:
		void OnAttach() override;
		void OnUpdate(DeltaTime dt) override;
		void OnDetach() override;

		void OnBeginScene();
		void OnEndScene();

		static Input GameInput;
		void OnEvent(Event& event) override;
		void OnNewFrame() override;
		void OnPreRender() override;
		void OnRender(RenderPayload& payload) override;

		unsigned int CreateTexture(std::filesystem::path assetPath);
		unsigned int GetTextureRenderId(UUID textureId);
		UUID LoadTextureFromAsset(std::filesystem::path assetPath);
		void SetRenderTargetDefault();
		void SetRenderTargetFrameBuffer();
		void ReloadShaders();

		void FullScreenPass(UUID materialInstanceId);
		void CopyActiveFrameBufferToDefaultFrameBuffer();

		void GetEntityMatPixelData(int x, int y, void* pixelData);
	private:
		friend class EngineAPI;
		RuntimeState m_CurrentRuntimeState = RuntimeState::STOPPED;
		enum class FrameBufferEnum {
			ONE = 0, TWO
		} m_CurrentFrameBuffer = FrameBufferEnum::ONE;
		IcePickRenderer::FrameBuffer m_FrameBufferOne;
		IcePickRenderer::FrameBuffer m_FrameBufferTwo;
		IcePickRenderer::FrameBuffer& GetFrameBuffer(FrameBufferEnum frameBuffer);

		AssetLoader m_AssetLoader;
		ScriptRunner m_ScriptRunner;
		PhysicsSystem3D m_PhysicsSystem3D;
		void RenderEntityMeshes();
		void RenderMeshNode(const IcePickRenderer::MeshNode& parent, glm::mat4 parentTransform, const std::vector<UUID>& materialSlots, const entt::entity entityId);
		void ProcessMaterialsAndPipeline(MaterialBase& materialBase, MaterialInstance& materialInstance);
		void PostProcessPass();

		bool m_RenderDebugPhysics = false;
		Scene m_CurrentScene;
		entt::entity m_SceneCameraId = entt::null;

		std::vector<IcePickRenderer::Texture> m_RenderTextureStorage;
	};
}