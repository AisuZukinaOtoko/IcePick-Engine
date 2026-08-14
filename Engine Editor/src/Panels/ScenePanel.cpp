#include "ScenePanel.h"
#include "Render Systems/Mesh.h"
#include "Scene Systems/Components.h"
#include "LogSystem.h"
#include <IconsFontAwesome7.h>

static const char* GetListItemIcon(const IcePick::TagComponent& tag) {
	switch (tag.Type) {
	case IcePick::TagComponent::EntityType::CAMERA:
		return ICON_FA_CAMERA;
	case IcePick::TagComponent::EntityType::CAMERA_CONTROLLER:
		return ICON_FA_VIDEO;
	case IcePick::TagComponent::EntityType::POINT_LIGHT:
		return ICON_FA_LIGHTBULB;
	case IcePick::TagComponent::EntityType::DIRECTIONAL_LIGHT:
		return ICON_FA_SUN;
	case IcePick::TagComponent::EntityType::TERRAIN:
		return ICON_FA_MAP;
	default:
		return ICON_FA_CUBE;
	}
}

ScenePanel::ScenePanel() {
	m_Title = "Scene Hierarchy";
}

void ScenePanel::SetSelectionContextChangeCallback(std::function<void(SelectionContext)> callback) {
	SelectionContextChangeCallback = callback;
}

void ScenePanel::SetSelectionContext(SelectionContext selectionContext) {
	m_SelectionContext = selectionContext;
}

void ScenePanel::OnUpdate(DeltaTime dt) {
	entt::entity selectedEntity = static_cast<entt::entity>(m_SelectionContext.SelectionId);
	if (m_SelectionContextChanged) {
		SelectionContextChangeCallback(m_SelectionContext);
		m_SelectionContextChanged = false;
	}
}


void ScenePanel::ShowSceneHierarchy(IcePick::EngineAPI& engineAPI) {
	ImGui::Begin(m_Title);

	if (ImGui::BeginMenu(ICON_FA_SQUARE_PLUS " Add")) {
		if (ImGui::MenuItem(ICON_FA_CUBE " Entity")) {
			IcePick::NewEntity();
		}

		if (ImGui::MenuItem(ICON_FA_VIDEO " Camera controller")) {
			IcePick::NewCameraController();
		}

		ImGui::BeginDisabled(true);

		if (ImGui::MenuItem(ICON_FA_LIGHTBULB " Point light")) {
			IcePick::NewPointLight();
		}

		if (ImGui::MenuItem(ICON_FA_SUN " Directional light")) {
			IcePick::NewDirectionalLight();
		}

		if (ImGui::MenuItem(ICON_FA_MAP " Terrain")) {
			IcePick::NewTerrain();
		}
		ImGui::EndDisabled();

		ImGui::EndMenu();
	}

	entt::entity selectedEntity = static_cast<entt::entity>(m_SelectionContext.SelectionId);
	auto& activeSceneRegistry = IcePick::GetActiveSceneRegistry();
	auto taggedEntityView = activeSceneRegistry.view<IcePick::TagComponent>();

	// Panel entity items
	for (auto entity : taggedEntityView) {
		IcePick::TagComponent entityTag = activeSceneRegistry.get<IcePick::TagComponent>(entity);
		const char* icon = GetListItemIcon(entityTag);

		ImGuiTreeNodeFlags entityNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Leaf;
		entityNodeFlags |= (selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0;

		bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)entity, entityNodeFlags, "%s %s", icon, entityTag.value.c_str());

		if (ImGui::IsItemClicked()) {
			m_SelectionContext.SelectionType = SelectionContext::Type::ENTITY;
			m_SelectionContext.SelectionId = static_cast<uint64_t>(entity);
		}

		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
			m_IsDraggingItem = true;
		}

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && ImGui::IsItemHovered()) {
			if (!m_IsDraggingItem)
				m_SelectionContextChanged = true;
		}

		if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
			m_SelectionContext.SelectionType = SelectionContext::Type::ENTITY;
			m_SelectionContext.SelectionId = static_cast<uint64_t>(entity);
			m_SelectionContextChanged = true;
			ImGui::OpenPopup("Options");
		}

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
			m_DraggedEntity = entity;
			std::string sceneObjectType = "ERROR";
			if (entityTag.Type == IcePick::TagComponent::EntityType::ENTITY)
				sceneObjectType = "SCENE_ENTITY";
			else if (entityTag.Type == IcePick::TagComponent::EntityType::CAMERA_CONTROLLER)
				sceneObjectType = "CAMERA_CONTROLLER";

			SelectionContext dragDropContext{ SelectionContext::Type::ENTITY, static_cast<uint64_t>(entity), 0};
			ImGui::SetDragDropPayload(sceneObjectType.c_str(), &dragDropContext, sizeof(SelectionContext), ImGuiCond_Once);
			ImGui::Text("%s %s", icon, entityTag.value.c_str());
			ImGui::EndDragDropSource();
		}

		if ((entityTag.Type != IcePick::TagComponent::EntityType::CAMERA) && ImGui::BeginPopup("Options"))
		{
			if ((entityTag.Type == IcePick::TagComponent::EntityType::ENTITY) && ImGui::BeginMenu("Add component"))
			{
				if (ImGui::MenuItem("Mesh renderer component")) {
					IcePick::AddComponent<IcePick::MeshRendererComponent>(selectedEntity);
				}

				if (ImGui::MenuItem("Script component")) {
					IcePick::AddComponent<IcePick::ScriptComponent>(selectedEntity);
				}

				if (ImGui::MenuItem("Rigid body component")) {
					IcePick::AddComponent<IcePick::RigidBodyComponent>(selectedEntity);
				}

				if (ImGui::MenuItem("IK solver component")) {
					IcePick::AddComponent<IcePick::IKSolverComponent>(selectedEntity);
				}

				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Delete")) {
				IcePick::DeleteEntity(selectedEntity);
				m_SelectionContext.SelectionType = SelectionContext::Type::ENTITY;
				m_SelectionContext.SelectionId = static_cast<uint64_t>(entt::null); // Deselect if deleted
				SelectionContextChangeCallback(m_SelectionContext);
			}

			ImGui::EndPopup();
		}

		if (IcePick::HasComponent<IcePick::MeshRendererComponent>(entity) && nodeOpen) {
			IcePick::MeshRendererComponent& meshRenderer = IcePick::GetComponent<IcePick::MeshRendererComponent>(entity);
			if (meshRenderer.MeshType == IcePick::ImportSettings::MeshType::SKELETAL_MESH) {
				IcePickRenderer::SkinnedMeshData& skinnedMeshData = engineAPI.GetSkinnedMeshDataById(meshRenderer.meshDataId);
				IcePick::Skeleton& meshSkeleton = engineAPI.GetSkeletonById(skinnedMeshData.SkeletonId);
				RenderSkeletonHierarchyRecursive(meshSkeleton.RootBone, meshSkeleton, entity);
			}
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

void ScenePanel::RenderSkeletonHierarchyRecursive(const IcePick::SkeletonNodeHierarchy& currentNode, IcePick::Skeleton& skeleton, entt::entity entityId) {
	ImGuiTreeNodeFlags isLeafFlag = (currentNode.Children.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0;
	ImGuiTreeNodeFlags isSelectedFlag = ((static_cast<uint64_t>(currentNode.NodeIndex) == m_SelectionContext.SelectionData) && (m_SelectionContext.SelectionType == SelectionContext::Type::BONE)) ? ImGuiTreeNodeFlags_Selected : 0;
	ImGuiTreeNodeFlags boneNodeFlags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | isLeafFlag | isSelectedFlag;

	ImGui::PushID(currentNode.NodeIndex);
	bool nodeOpen = false;
	bool isBone = currentNode.BoneIndex != -1;
	const char* icon = (isBone) ? ICON_FA_BONE : ICON_FA_CUBES;

	bool boneSelected = isBone && (m_SelectionContext.SelectionData == static_cast<uint64_t>(currentNode.BoneLocalTransformIndex));
	boneNodeFlags |= (boneSelected) ? ImGuiTreeNodeFlags_Selected : 0;

	if (isBone || currentNode.Children.size()) {
		nodeOpen = ImGui::TreeNodeEx("Bone", boneNodeFlags, "%s %s", icon, currentNode.NodeName.c_str());
		if (ImGui::IsItemClicked() && isBone) {
			m_SelectionContext.SelectionType = SelectionContext::Type::BONE;
			m_SelectionContext.SelectionId = static_cast<uint64_t>(entityId);
			m_SelectionContext.SelectionData = static_cast<uint64_t>(currentNode.NodeIndex);
			m_SelectionContextChanged = true;
			IP_LOG(std::to_string(m_SelectionContext.SelectionData));
		}
	}

	if (nodeOpen) {
		for (unsigned int i = 0; i < currentNode.Children.size(); i++) {
			RenderSkeletonHierarchyRecursive(currentNode.Children[i], skeleton, entityId);
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
}

entt::entity ScenePanel::GetDraggedEntity() {
	return m_DraggedEntity;
}

ScenePanel::~ScenePanel() {

}

