#include "ScenePanel.h"
#include "Scene Systems/Components.h"
#include <IconsFontAwesome4.h>

static const char* GetListItemIcon(const IcePick::TagComponent& tag) {
	switch (tag.Type) {
	case IcePick::TagComponent::EntityType::CAMERA:
		return ICON_FA_CAMERA;
	case IcePick::TagComponent::EntityType::CAMERA_CONTROLLER:
		return ICON_FA_VIDEO_CAMERA;
	case IcePick::TagComponent::EntityType::POINT_LIGHT:
		return ICON_FA_LIGHTBULB_O;
	case IcePick::TagComponent::EntityType::DIRECTIONAL_LIGHT:
		return ICON_FA_SUN_O;
	case IcePick::TagComponent::EntityType::TERRAIN:
		return ICON_FA_MAP;
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

	if (ImGui::BeginMenu(ICON_FA_PLUS " Add")) {
		if (ImGui::MenuItem(ICON_FA_CUBE " Entity")) {
			IcePick::NewEntity();
		}

		if (ImGui::MenuItem(ICON_FA_VIDEO_CAMERA " Camera controller")) {
			IcePick::NewCameraController();
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
		}

		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
			m_IsDraggingItem = true;
		}

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImGui::IsItemHovered()) {
			if (!m_IsDraggingItem)
				m_EntitySelected = true;
		}

		if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
			m_SelectedEntity = entity;
			m_EntitySelected = true;
			ImGui::OpenPopup("Options");
		}

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
			m_DraggedEntity = entity;
			std::string sceneObjectType = "ERROR";
			if (entityTag.Type == IcePick::TagComponent::EntityType::ENTITY)
				sceneObjectType = "SCENE_ENTITY";
			else if (entityTag.Type == IcePick::TagComponent::EntityType::CAMERA_CONTROLLER)
				sceneObjectType = "CAMERA_CONTROLLER";
			ImGui::SetDragDropPayload(sceneObjectType.c_str(), nullptr, 0, ImGuiCond_Once);
			ImGui::Text("%s %s", icon, entityTag.value.c_str());
			ImGui::EndDragDropSource();
		}

		if ((entityTag.Type != IcePick::TagComponent::EntityType::CAMERA) && ImGui::BeginPopup("Options"))
		{
			if ((entityTag.Type == IcePick::TagComponent::EntityType::ENTITY) && ImGui::BeginMenu("Add component"))
			{
				if (ImGui::MenuItem("Mesh renderer component")) {
					IcePick::AddComponent<IcePick::MeshRendererComponent>(m_SelectedEntity);
				}

				if (ImGui::MenuItem("Script component")) {
					IcePick::AddComponent<IcePick::ScriptComponent>(m_SelectedEntity);
				}

				if (ImGui::MenuItem("Rigid body component")) {
					IcePick::AddComponent<IcePick::RigidBodyComponent>(m_SelectedEntity);
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

	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
		m_IsDraggingItem = false;
	}

	ImGui::End();
}

bool ScenePanel::EntitySelected() {
	return m_EntitySelected;
}

entt::entity ScenePanel::GetDraggedEntity() {
	return m_DraggedEntity;
}

entt::entity ScenePanel::GetSelectedEntity() {
	return m_SelectedEntity;
}

ScenePanel::~ScenePanel() {

}

