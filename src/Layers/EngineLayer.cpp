#include "../Render Systems/Renderer.h"
#include "EngineLayer.h"
#include "../LogSystem.h"
#include "../Event Systems/Input.h"
#include "../Scene Systems/SceneRegistry.h"
#include "../Vendor/glm/gtc/matrix_transform.hpp"

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


void IcePick::EngineLayer::OnEvent(Event& event) {

	if ((event.action == IP_PRESS) && (event.code == IP_KEY_S) && (event.mods & GLFW_MOD_SHIFT)) {
		IP_LOG("Shift-S pressed.");
		event.flags |= EVENT_HANDLED;
	}

	gameInput.OnEvent(event);
}

void IcePick::EngineLayer::OnRender(RenderPayload& payload) {
	m_FrameBuffer.Bind();
	m_CurrentScene.OnPreRender();
	payload.FrameBufferID = m_FrameBuffer.GetColourTextureID();
	RenderEntityMeshes();
	m_FrameBuffer.UnBind();
}

void IcePick::EngineLayer::RenderEntityMeshes() {
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

		if (!EntityMeshRendererComponent.MeshLoaded && !EntityMeshRendererComponent.MeshFilePath.empty()) {
			EntityMeshRendererComponent = m_AssetLoader.LoadMesh(EntityMeshRendererComponent.MeshFilePath);
		}

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
				Material tempMaterial;
				tempMaterial.ShaderID = 1;
				tempMaterial.AlbedoMap = 1;
				tempMaterial.NormalMap = 0;
				tempMaterial.RoughnessMap = 0;
				tempMaterial.MetallicMap = 0;
				tempMaterial.EmissiveMap = 0;

				tempMaterial.Albedo = glm::vec3(1.0f, 1.0f, 0.0f);
				tempMaterial.SpecularColour = glm::vec3(0.0f, 1.0f, 1.0f);
				tempMaterial.SampleBitmask = Material::ALBEDO;
				IcePickRenderer::DrawMesh(mesh, model, tempMaterial);
			}
			else {
				IcePickRenderer::DrawMeshBasicMaterial(mesh, model);
			}
		}
	}

}

void IcePick::EngineLayer::OnDetach() {

}