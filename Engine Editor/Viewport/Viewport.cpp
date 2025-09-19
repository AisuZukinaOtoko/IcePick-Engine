#include "Viewport.h"
#include "../Render Systems/Renderer.h"
#include "../Event Systems/Input.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_glfw.h"
#include "imgui-docking/imgui_impl_opengl3.h"
#include "../Scene Systems/SceneRegistry.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../LogSystem.h"

static IcePick::Input keyState;
static int temp = 0;

Viewport::Viewport(IcePick::EngineAPI engineAPI) :
	m_EngineAPI(engineAPI)
{
	m_ViewportSize = ImVec2(1920, 180);
}

void Viewport::SetSelectedEntityChangeCallback(std::function<void(entt::entity)> callback) {
	SelectedEntityChangeCallback = callback;
}

void Viewport::SetSelectedEntity(entt::entity entity) {
	m_SelectedEntity = entity;
}

void Viewport::SetDropAssetPath(std::string filePath) {
	m_DropAssetPath = filePath;
}

void Viewport::OnUpdate(DeltaTime dt) {
	if (m_EntitySelected) {
		SelectedEntityChangeCallback(m_SelectedEntity);
		m_EntitySelected = false;
	}

	m_EditorCamera.aspectRatio = m_ViewportSize.x / (float)m_ViewportSize.y;
	m_EditorCamera.yaw += m_MouseDelta.x * 0.2;
	m_EditorCamera.pitch += m_MouseDelta.y * 0.152;
	m_EditorCamera.OnUpdate(dt);
	glm::mat4 viewProjMatrix = m_EditorCamera.GetViewProjectionMatrix();
	IcePickRenderer::SetRenderViewProjectionMatrix(viewProjMatrix);
	IcePickRenderer::SetRenderCameraWorldPosition(m_EditorCamera.cameraPosition);
}

void Viewport::OnViewportEvent(IcePick::Event& event) {
	keyState.OnEvent(event);

	if (keyState.IsKeyPressed(IcePick::IP_KEY_P)) {
		temp++;
	}
	if (keyState.IsKeyPressed(IcePick::IP_KEY_O)) {
		temp--;
	}

	if (m_SelectedEntity != entt::null) {
		if (keyState.IsKeyPressed(IcePick::IP_KEY_1))
			m_GizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		else if (keyState.IsKeyPressed(IcePick::IP_KEY_2))
			m_GizmoOperation = ImGuizmo::OPERATION::ROTATE;
		else if (keyState.IsKeyPressed(IcePick::IP_KEY_3))
			m_GizmoOperation = ImGuizmo::OPERATION::SCALE;
	}

	if (m_ViewportRightClicked) {
		m_EditorCamera.OnKeyPress(event.action, event.code);
	}
}

void Viewport::Render(unsigned int renderTexture) {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin(m_ID);

	m_WindowPosition = ImGui::GetWindowPos();
	m_ViewportSize = ImGui::GetContentRegionAvail();
	ImVec2 regionMin = ImGui::GetWindowContentRegionMin();
	m_WindowPosition = ImVec2(m_WindowPosition.x + regionMin.x, m_WindowPosition.y + regionMin.y);
	ImVec2 mousePos = ImGui::GetMousePos();
	m_WindowMousePosition = ImVec2(mousePos.x - m_WindowPosition.x, mousePos.y - m_WindowPosition.y);

	ImGui::Image((void*)(intptr_t)(renderTexture + temp), m_ViewportSize, ImVec2(0, 1), ImVec2(1, 0));
	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET")) {
			//std::filesystem::path droppedAssetPath(m_DropAssetPath);
			IP_LOG(m_DropAssetPath.string() + " dropped on viewport");
			uint32_t pixelData[2] = { 0, 0 };
			m_EngineAPI.GetEntityMatPixelData(m_WindowMousePosition.x, m_ViewportSize.y - m_WindowMousePosition.y, pixelData);
			std::cout << pixelData[0] << " " << pixelData[1] << std::endl;
		}
		ImGui::EndDragDropTarget();
	}

	ImGuiIO& io = ImGui::GetIO();

	if (m_SelectedEntity != entt::null) {
		RenderEntityGizmos();
	}


	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
		m_ViewportRightClicked = true;
		IcePickRenderer::RequestCursorLock();
	}

	if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
		m_ViewportRightClicked = false;
		IcePickRenderer::RequestCursorUnlock();
	}

	m_MouseDelta = (m_ViewportRightClicked) ? io.MouseDelta : ImVec2(0.0f, 0.0f);
	ImGui::End();
	ImGui::PopStyleVar();
}

void Viewport::RenderEntityGizmos() {
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();
	
	ImGuizmo::SetRect(m_WindowPosition.x, m_WindowPosition.y, m_ViewportSize.x, m_ViewportSize.y);

	IcePick::TransformComponent& entityTransform = IcePick::GetComponent<IcePick::TransformComponent>(m_SelectedEntity);
	glm::mat4 entityTransformMatrix = glm::mat4(1.0f);
	entityTransformMatrix = glm::translate(entityTransformMatrix, entityTransform.Position);
	glm::quat q = glm::quat(glm::radians(entityTransform.Rotation));
	entityTransformMatrix *= glm::toMat4(q);
	entityTransformMatrix = glm::scale(entityTransformMatrix, entityTransform.Scale);

	glm::mat4 cameraViewMatrix = m_EditorCamera.GetViewMatrix();
	glm::mat4 cameraProjectionMatrix = m_EditorCamera.GetProjectionMatrix();

	ImGuizmo::Manipulate(
		glm::value_ptr(cameraViewMatrix),
		glm::value_ptr(cameraProjectionMatrix),
		m_GizmoOperation,
		ImGuizmo::LOCAL,
		glm::value_ptr(entityTransformMatrix)
	);

	if (ImGuizmo::IsUsing()) {
		glm::vec3 translation, scale, skew;
		glm::vec4 perspective;
		glm::quat rotation;

		glm::decompose(entityTransformMatrix, scale, rotation, translation, skew, perspective);

		entityTransform.Position = translation;
		entityTransform.Scale = scale;
		glm::vec3 euler = glm::eulerAngles(rotation);
		entityTransform.Rotation = glm::degrees(euler);;
	}

}

Viewport::~Viewport() {

}

