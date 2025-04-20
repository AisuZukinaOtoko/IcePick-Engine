#include "ScenePanel.h"

ScenePanel::ScenePanel() {
	m_Title = "Scene Hierarchy";
}


void ScenePanel::ShowSceneHierarchy() {
	auto& activeSceneRegistry = IcePick::GetActiveSceneRegistry();
	auto taggedEntityView = activeSceneRegistry.view<IcePick::TagComponent>();

	ImGui::Begin(m_Title);
	// Panel entity items
	for (auto entity : taggedEntityView) {
		IcePick::TagComponent entityTag = activeSceneRegistry.get<IcePick::TagComponent>(entity);

		ImGuiTreeNodeFlags entityNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow |	ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Leaf;
		entityNodeFlags |= (m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0;

		bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)entity, entityNodeFlags, "%s", entityTag.value.c_str());

		if (ImGui::IsItemClicked()) {
			m_SelectedEntity = entity;
			m_EntitySelected = true;
			IP_LOG("Entity selected.");
		}

		if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
			m_SelectedEntity = entity;
			m_EntitySelected = true;
			ImGui::OpenPopup("EntityOptions");
		}

		if (ImGui::BeginPopup("EntityOptions"))
		{
			if (ImGui::BeginMenu("Add component"))
			{
				if (ImGui::MenuItem("Mesh component")) {
					IcePick::AddComponent<IcePick::MeshComponent>(m_SelectedEntity);
				}

				if (ImGui::MenuItem("Material component")) {
					IcePick::AddComponent<IcePick::MaterialComponent>(m_SelectedEntity);
				}

				if (ImGui::MenuItem("Rigid body component")) {
					IcePick::AddComponent<IcePick::RigidBodyComponent>(m_SelectedEntity);
				}

				if (ImGui::MenuItem("Sphere collider component")) {
					IcePick::AddComponent<IcePick::SphereColliderComponent>(m_SelectedEntity);
				}
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Delete Entity")) {
				IcePick::DeleteEntity(m_SelectedEntity);
				m_SelectedEntity = entt::null; // Deselect if deleted
			}

			if (ImGui::MenuItem("Increment Entity Vertex Array")) {
				IcePick::MeshComponent& mesh = IcePick::GetComponent<IcePick::MeshComponent>(m_SelectedEntity);
				mesh.MeshVertexArrayRegistryIndex++;
			}

			ImGui::EndPopup();
		}

		if (nodeOpen) {
			ImGui::TreePop();
		}
	}

	if (ImGui::Button("New Entity")) {
		IcePick::NewEntity();
	}

	ImGui::End();
}

bool ScenePanel::EntitySelected() {
	return m_EntitySelected;
}

entt::entity ScenePanel::GetSelectedEntity() {
	return m_SelectedEntity;
}

ScenePanel::~ScenePanel() {

}