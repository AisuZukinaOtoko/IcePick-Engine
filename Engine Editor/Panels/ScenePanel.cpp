#include "ScenePanel.h"
#include "IconsFontAwesome4.h"

static const char* GetListItemIcon(const IcePick::TagComponent& tag) {
	switch (tag.Type) {
	case IcePick::TagComponent::EntityType::POINT_LIGHT:
		return ICON_FA_LIGHTBULB_O;
		break;
	case IcePick::TagComponent::EntityType::DIRECTIONAL_LIGHT:
		return ICON_FA_SUN_O;
		break;
	case IcePick::TagComponent::EntityType::TERRAIN:
		return ICON_FA_MAP;
		break;
	default:
		return ICON_FA_CUBE;
	}
}

ScenePanel::ScenePanel() {
	m_Title = "Scene Hierarchy";
}

void ScenePanel::SetSelectedEntityChangeCallback(std::function<void(entt::entity)> callback) {
	SelectedEntityChangeCallback = callback;
}

void ScenePanel::SetSelectedEntity(entt::entity entity) {
	m_SelectedEntity = entity;
}

void ScenePanel::OnUpdate(DeltaTime dt) {
	if (m_EntitySelected) {
		SelectedEntityChangeCallback(m_SelectedEntity);
		m_EntitySelected = false;
	}
}


void ScenePanel::ShowSceneHierarchy() {
	ImGui::Begin(m_Title);

	/*if (ImGui::Button(ICON_FA_PLUS " Add")) {
		IcePick::NewEntity();
	}*/

	if (ImGui::BeginMenu(ICON_FA_PLUS " Add")) {
		if (ImGui::MenuItem(ICON_FA_CUBE " Entity")) {
			IcePick::NewEntity();
		}

		if (ImGui::MenuItem(ICON_FA_LIGHTBULB_O " Point light")) {
			IcePick::NewPointLight();
		}

		if (ImGui::MenuItem(ICON_FA_SUN_O " Directional light")) {
			IcePick::NewDirectionalLight();
		}

		if (ImGui::MenuItem(ICON_FA_MAP " Terrain")) {
			IcePick::NewTerrain();
		}
		ImGui::EndMenu();
	}

	auto& activeSceneRegistry = IcePick::GetActiveSceneRegistry();
	auto taggedEntityView = activeSceneRegistry.view<IcePick::TagComponent>();

	// Panel entity items
	for (auto entity : taggedEntityView) {
		IcePick::TagComponent entityTag = activeSceneRegistry.get<IcePick::TagComponent>(entity);
		const char* icon = GetListItemIcon(entityTag);

		ImGuiTreeNodeFlags entityNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow |	ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Leaf;
		entityNodeFlags |= (m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0;

		bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)entity, entityNodeFlags, "%s %s", icon, entityTag.value.c_str());

		if (ImGui::IsItemClicked()) {
			m_SelectedEntity = entity;
			m_EntitySelected = true;
		}

		if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
			m_SelectedEntity = entity;
			m_EntitySelected = true;
			ImGui::OpenPopup("Options");
		}

		if (ImGui::BeginPopup("Options"))
		{
			if ((entityTag.Type == IcePick::TagComponent::EntityType::ENTITY) && ImGui::BeginMenu("Add component"))
			{
				if (ImGui::MenuItem("Mesh renderer component")) {
					IcePick::AddComponent<IcePick::MeshRendererComponent>(m_SelectedEntity);
				}

				if (ImGui::MenuItem("Script component")) {
					IcePick::AddComponent<IcePick::ScriptComponent>(m_SelectedEntity);
				}

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Delete")) {
				IcePick::DeleteEntity(m_SelectedEntity);
				m_SelectedEntity = entt::null; // Deselect if deleted
				SelectedEntityChangeCallback(entt::null);
			}

			ImGui::EndPopup();
		}

		if (nodeOpen) {
			ImGui::TreePop();
		}
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

