#include "../Render Systems/Renderer.h"
#include "GameLayer.h"
#include "../LogSystem.h"
#include "../Event Systems/Input.h"
#include "../Scene Systems/SceneRegistry.h"
#include "../Vendor/glm/gtc/matrix_transform.hpp"

static IcePick::Input gameInput;

void IcePick::GameLayer::OnAttach() {
	m_FrameBuffer.Init();
	m_CurrentScene.OnBegin();
	m_CurrentScene.LoadFromDisk("res/Assets/hatsune_miku.glb");
	m_CurrentScene.LoadFromDisk("res/Assets/hatsune_miku_fortnite.glb");
}

void IcePick::GameLayer::OnUpdate(DeltaTime dt) {
	if (gameInput.IsKeyPressed(IP_KEY_T)) {
		IP_LOG("Game layer pressed T.");
	}

	m_CurrentScene.OnUpdate(dt);
	temp_DeleteLater++;
}


void IcePick::GameLayer::OnEvent(Event& event) {

	if ((event.action == IP_PRESS) && (event.code == IP_KEY_S) && (event.mods & GLFW_MOD_SHIFT)) {
		IP_LOG("Shift-S pressed.");
		event.flags |= EVENT_HANDLED;
	}

	gameInput.OnEvent(event);
}

void IcePick::GameLayer::OnRender(RenderPayload& payload) {
	m_FrameBuffer.Bind();
	m_CurrentScene.OnPreRender();
	payload.FrameBufferID = m_FrameBuffer.GetID();
	RenderEntityMeshes();
	m_FrameBuffer.UnBind();
}

void IcePick::GameLayer::RenderEntityMeshes() {
	float angle = glm::radians(45.0f);
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat3 normalMatrix = glm::mat3(1.0f);

	auto& ActiveSceneRegistry = GetActiveSceneRegistry();
	auto sceneView = ActiveSceneRegistry.view<MeshRendererComponent>();

	for (entt::entity entity: sceneView) {
		MeshRendererComponent& EntityMeshRendererComponent = ActiveSceneRegistry.get<MeshRendererComponent>(entity);
		TransformComponent& EntityTransformComponent = ActiveSceneRegistry.get<TransformComponent>(entity);

		if (!EntityMeshRendererComponent.MeshVisible)
			continue;

		model = glm::mat4(1.0f);
		model = glm::translate(model, EntityTransformComponent.Position);
		model = glm::rotate(model, glm::radians(EntityTransformComponent.Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(EntityTransformComponent.Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(EntityTransformComponent.Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, EntityTransformComponent.Scale);
		normalMatrix = glm::mat3(1.0f);
		normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

		IcePickRenderer::SetRenderWorldNormalMatrix(normalMatrix);

		for (int i = 0; i < EntityMeshRendererComponent.MeshCount; i++) {
			MeshComponent& mesh = EntityMeshRendererComponent.Meshes[i];

			if (mesh.MaterialIndex != -1) {
				IcePickRenderer::DrawMesh(mesh, model);
			}
			else {
				IcePickRenderer::DrawMeshBasicMaterial(mesh, model);
			}
		}
	}

}

void IcePick::GameLayer::OnDetach() {

}