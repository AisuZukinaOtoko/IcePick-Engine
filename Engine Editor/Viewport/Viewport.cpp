#include "Viewport.h"
#include "../Render Systems/Renderer.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_glfw.h"
#include "imgui-docking/imgui_impl_opengl3.h"
#include "imgui-docking/ImGuizmo.h"
#include "../Scene Systems/SceneRegistry.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../LogSystem.h"

Viewport::Viewport() {
	m_ViewportSize = ImVec2(1920, 180);
}

void Viewport::SetSelectedEntityChangeCallback(std::function<void(entt::entity)> callback) {
	SelectedEntityChangeCallback = callback;
}

void Viewport::SetSelectedEntity(entt::entity entity) {
	m_SelectedEntity = entity;
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
	if (m_ViewportRightClicked) {
		m_EditorCamera.OnKeyPress(event.action, event.code);
	}
}

void Viewport::Render(unsigned int frameBuffer) {
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin(m_ID);
	m_WindowPosition = ImGui::GetWindowPos();
	m_ViewportSize = ImGui::GetContentRegionAvail();
	ImGui::Image((void*)(intptr_t)frameBuffer, m_ViewportSize, ImVec2(0, 1), ImVec2(1, 0));
	ImGuiIO& io = ImGui::GetIO();

	if (m_SelectedEntity != entt::null) {
		RenderEntityGuizmos();
	}


	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && io.WantCaptureMouse) {
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

void Viewport::RenderEntityGuizmos() {
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();
	
	ImGuizmo::SetRect(m_WindowPosition.x, m_WindowPosition.y, m_ViewportSize.x, m_ViewportSize.y);

	IcePick::TransformComponent& entityTransform = IcePick::GetComponent<IcePick::TransformComponent>(m_SelectedEntity);
	glm::mat4 entityTransformMatrix = glm::mat4(1.0f);
	entityTransformMatrix = glm::translate(entityTransformMatrix, entityTransform.Position);
	entityTransformMatrix = glm::rotate(entityTransformMatrix, glm::radians(entityTransform.Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	entityTransformMatrix = glm::rotate(entityTransformMatrix, glm::radians(entityTransform.Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	entityTransformMatrix = glm::rotate(entityTransformMatrix, glm::radians(entityTransform.Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	entityTransformMatrix = glm::scale(entityTransformMatrix, entityTransform.Scale);

	glm::mat4 cameraViewMatrix = m_EditorCamera.GetViewMatrix();
	glm::mat4 cameraProjectionMatrix = m_EditorCamera.GetProjectionMatrix();

	ImGuizmo::Manipulate(
		glm::value_ptr(cameraViewMatrix),      // view matrix
		glm::value_ptr(cameraProjectionMatrix),// projection matrix
		ImGuizmo::OPERATION::ROTATE,                  // operation (translate, rotate, scale)
		ImGuizmo::LOCAL,                   // mode (local or world space)
		glm::value_ptr(entityTransformMatrix)          // the matrix to manipulate
	);

}

Viewport::~Viewport() {

}

