#include "Viewport.h"
#include "../Render Systems/Renderer.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_glfw.h"
#include "imgui-docking/imgui_impl_opengl3.h"
#include "../LogSystem.h"

Viewport::Viewport() {
	m_ViewportSize = ImVec2(1920, 180);
}

void Viewport::OnUpdate(DeltaTime dt) {
	m_EditorCamera.aspectRatio = m_ViewportSize.x / (float)m_ViewportSize.y;
	m_EditorCamera.yaw += m_MouseDelta.x * 0.2;
	m_EditorCamera.pitch += m_MouseDelta.y * 0.152;
	m_EditorCamera.OnUpdate(dt);
	glm::mat4 viewProjMatrix = m_EditorCamera.getViewProjectionMatrix();
	IcePickRenderer::SetRenderViewProjectionMatrix(viewProjMatrix);
}

void Viewport::OnViewportEvent(IcePick::Event& event) {
	if (m_ViewportRightClicked) {
		m_EditorCamera.OnKeyPress(event.action, event.code);
	}
}

void Viewport::Render(unsigned int frameBuffer) {
	ImGui::Begin(m_ID);
	m_ViewportSize = ImGui::GetContentRegionAvail();
	ImGui::Image((void*)(intptr_t)frameBuffer, m_ViewportSize);
	ImGuiIO& io = ImGui::GetIO();


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
}

Viewport::~Viewport() {

}

