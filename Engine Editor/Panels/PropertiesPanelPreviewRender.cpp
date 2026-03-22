#include "PropertiesPanel.h"
#include "../Scene Systems/SceneRegistry.h"

static void PreviewMeshRendererComponent(IcePick::MeshRendererComponent& meshRenderer, EditorRenderer& renderer) {
	glm::mat4 modelMatrix = glm::identity<glm::mat4>();
	renderer.RenderMesh(meshRenderer, modelMatrix);
}

static void PreviewRigidBodyColliders(IcePick::RigidBodyComponent& rigidBody, EditorRenderer& renderer) {

}

void PropertiesPanel::PreviewRender() {
	ImGui::Spacing();
	ImGui::SeparatorText("Preview");

	using namespace IcePick;
	m_PreviewRenderer.Clear();

	m_PreviewRenderer.Use();
	if (!HasComponent<TagComponent>(m_SelectedEntity)) {
		return;
	}

	//TagComponent& tag = GetComponent<TagComponent>(m_SelectedEntity);
	//bool isPrefabInstance = (tag.Type == TagComponent::EntityType::PREFAB);
	//entt::registry& registry = (isPrefabInstance) ? GetActivePrefabRegistry() : GetActiveSceneRegistry();
	entt::entity entityId = m_SelectedEntity;

	//if (isPrefabInstance) {
		
	//}
	if (HasComponent<MeshRendererComponent>(entityId)) {
		MeshRendererComponent& meshRenderer = GetComponent< MeshRendererComponent>(entityId);
		PreviewMeshRendererComponent(meshRenderer, m_PreviewRenderer);
	}

	ImVec2 availableSapce = ImGui::GetContentRegionAvail();
	ImGui::Image((void*)m_PreviewRenderer.GetRenderTexture(), ImVec2(availableSapce.x, availableSapce.x), ImVec2(0, 1), ImVec2(1, 0));


	m_PreviewRenderer.UnBind();
}