#include "../Render Systems/Renderer.h"
#include "EngineLayer.h"
#include "../LogSystem.h"
#include "../Event Systems/Input.h"
#include "../Scene Systems/SceneRegistry.h"
#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"


static IcePick::Input gameInput;

void IcePick::EngineLayer::OnAttach() {
	m_FrameBuffer.Init();
	m_CurrentScene.OnBegin();
}

void IcePick::EngineLayer::OnUpdate(DeltaTime dt) {
	if (gameInput.IsKeyPressed(IP_KEY_T)) {
		IP_LOG("Game layer pressed T.");
	}

	m_CurrentScene.OnUpdate(dt);
	temp_DeleteLater++;
}

void IcePick::EngineLayer::OnNewFrame() {
	m_FrameBuffer.Clear();
}

void IcePick::EngineLayer::OnEvent(Event& event) {

	if ((event.action == IP_PRESS) && (event.code == IP_KEY_S) && (event.mods & GLFW_MOD_SHIFT)) {
		IP_LOG("Shift-S pressed.");
		event.flags |= EVENT_HANDLED;
	}

	gameInput.OnEvent(event);
}

unsigned int IcePick::EngineLayer::CreateTexture(std::filesystem::path texturePath) {
	return m_AssetLoader.LoadTexture(texturePath);
}

void IcePick::EngineLayer::SetRenderTargetDefault() {
	m_FrameBuffer.UnBind();
}

void IcePick::EngineLayer::SetRenderTargetFrameBuffer() {
	m_FrameBuffer.Bind();
}

void IcePick::EngineLayer::OnRender(RenderPayload& payload) {
	SetRenderTargetFrameBuffer();
	m_CurrentScene.OnPreRender();
	payload.FrameBufferID = m_FrameBuffer.GetColourTextureID();
	RenderEntityMeshes();
}

void IcePick::EngineLayer::RenderEntityMeshes() {
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat3 normalMatrix = glm::mat3(1.0f);

	auto& ActiveSceneRegistry = GetActiveSceneRegistry();
	auto sceneView = ActiveSceneRegistry.view<MeshRendererComponent>();

	for (entt::entity entity: sceneView) {
		MeshRendererComponent& EntityMeshRendererComponent = ActiveSceneRegistry.get<MeshRendererComponent>(entity);
		TransformComponent& EntityTransformComponent = ActiveSceneRegistry.get<TransformComponent>(entity);

		if (!EntityMeshRendererComponent.MeshVisible)
			continue;

		if (!EntityMeshRendererComponent.MeshLoaded && !EntityMeshRendererComponent.MeshFilePath.empty()) {
			EntityMeshRendererComponent = m_AssetLoader.LoadMesh(EntityMeshRendererComponent.MeshFilePath);
		}

		model = glm::mat4(1.0f);
		model = glm::translate(model, EntityTransformComponent.Position);
		glm::quat q = glm::quat(glm::radians(EntityTransformComponent.Rotation));
		model *= glm::toMat4(q);
		model = glm::scale(model, EntityTransformComponent.Scale);
		normalMatrix = glm::mat3(1.0f);
		normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

		IcePickRenderer::SetRenderWorldNormalMatrix(normalMatrix);
		RenderMeshNode(EntityMeshRendererComponent.RootMeshNode, model, EntityMeshRendererComponent.MaterialSlots);
	}

}

void IcePick::EngineLayer::RenderMeshNode(const MeshNode& parent, glm::mat4 parentTransform, const std::vector<UUID>& materialSlots) {
	glm::mat4 meshWorldTransform = parentTransform * parent.NodeTransform;
	for (unsigned int vertexArrayID : parent.VertexArrayIDs) {
		MeshComponent mesh = { vertexArrayID, -1, MeshComponent::STATIC };

		UUID meshMaterialId = (parent.MaterialSlotIndex != -1) ? materialSlots[parent.MaterialSlotIndex] : UUID::Unitialised();
		const Material& material = m_AssetLoader.GetMaterial(meshMaterialId);

		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(meshWorldTransform)));
		IcePickRenderer::SetRenderWorldNormalMatrix(normalMatrix);
		IcePickRenderer::DrawMesh(mesh, meshWorldTransform, material);
	}

	for (const MeshNode& meshNode : parent.Children) {
		RenderMeshNode(meshNode, meshWorldTransform, materialSlots);
	}
}

void IcePick::EngineLayer::OnDetach() {
	m_AssetLoader.ShutDown();
}