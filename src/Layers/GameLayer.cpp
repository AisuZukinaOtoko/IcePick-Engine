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
	
	IcePick::MeshComponent temp = {};

	IcePickRenderer::DrawMeshBasicMaterial(temp, model);
	auto& ActiveSceneRegistry = GetActiveSceneRegistry();
	auto sceneView = ActiveSceneRegistry.view<MeshComponent>();

	for (entt::entity entity: sceneView) {
		MeshComponent& EntityMeshComponent = ActiveSceneRegistry.get<MeshComponent>(entity);
		TransformComponent& EntityTransformComponent = ActiveSceneRegistry.get<TransformComponent>(entity);

		model = glm::mat4(1.0f);
		model = glm::translate(model, EntityTransformComponent.Position);
		model = glm::scale(model, EntityTransformComponent.Scale);
		model = glm::rotate(model, EntityTransformComponent.Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, EntityTransformComponent.Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, EntityTransformComponent.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

		if (ActiveSceneRegistry.all_of<MaterialComponent>(entity)) {
			MaterialComponent& EntityMaterialComponent = ActiveSceneRegistry.get<MaterialComponent>(entity);
			IcePickRenderer::DrawMesh(EntityMeshComponent, EntityMaterialComponent, model);
		}
		else {
			IcePickRenderer::DrawMeshBasicMaterial(EntityMeshComponent, model);
			//IP_LOG("Draw me");
		}
	}

}

void IcePick::GameLayer::OnDetach() {

}