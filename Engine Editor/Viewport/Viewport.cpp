#include "Viewport.h"
#include "../Render Systems/Renderer.h"
#include "../Event Systems/Input.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_impl_glfw.h"
#include "imgui-docking/imgui_impl_opengl3.h"
#include "../Scene Systems/SceneRegistry.h"
#include "../Scene Systems/SceneCamera.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "IconsFontAwesome4.h"
#include "../LogSystem.h"
#include "../../src/Utilities/DebugStatistics.h"
#include <math.h>

static IcePick::Input keyState;

static void DrawDebugBox(glm::vec3 center, glm::quat rotation, glm::vec3 halfExtents) {
	IcePickRenderer::LinePointVertex3D corners[8];
	glm::vec4 colour = { 0.0f, 0.0f, 1.0f, 1.0f };

	float hx = halfExtents.x;
	float hy = halfExtents.y;
	float hz = halfExtents.z;

	corners[0] = { { -hx, -hy, -hz }, colour };
	corners[1] = { { hx, -hy, -hz }, colour };
	corners[2] = { { hx,  hy, -hz }, colour };
	corners[3] = { { -hx,  hy, -hz }, colour };

	corners[4] = { { -hx, -hy,  hz }, colour };
	corners[5] = { { hx, -hy,  hz }, colour };
	corners[6] = { { hx,  hy,  hz }, colour };
	corners[7] = { { -hx,  hy,  hz }, colour };

	for (int i = 0; i < 8; i++)
		corners[i].Position = rotation * (corners[i].Position) + center;

	DrawLine(corners[0], corners[1]);
	DrawLine(corners[1], corners[2]);
	DrawLine(corners[2], corners[3]);
	DrawLine(corners[3], corners[0]);

	DrawLine(corners[4], corners[5]);
	DrawLine(corners[5], corners[6]);
	DrawLine(corners[6], corners[7]);
	DrawLine(corners[7], corners[4]);

	DrawLine(corners[0], corners[4]);
	DrawLine(corners[1], corners[5]);
	DrawLine(corners[2], corners[6]);
	DrawLine(corners[3], corners[7]);
}

static void DrawDebugCircle(glm::vec3 center, glm::vec3 up,  glm::vec3 right, float radius) {
	glm::vec4 colour = { 0.0f, 0.0f, 1.0f, 1.0f };
	glm::vec3 forward = glm::cross(up, right);

	const int segments = 32;

	glm::vec3 prev =
		center +
		right * radius;

	for (int i = 1; i <= segments; i++)
	{
		float angle = (2.0f * 3.141593658f * i) / segments;

		glm::vec3 next = center + right * cos(angle) * radius +	forward * sin(angle) * radius;
		IcePickRenderer::LinePointVertex3D previousPoint = { prev, colour };
		IcePickRenderer::LinePointVertex3D nextPoint = { next, colour };
		DrawLine(previousPoint, nextPoint);
		prev = next;
	}
}

static void DrawDebugSphere(glm::vec3 center, float radius,	glm::vec3 cameraFront, glm::vec3 cameraUp, glm::vec3 cameraRight) {
	DrawDebugCircle(center, cameraUp, cameraRight, radius);
	DrawDebugCircle(center, cameraRight, cameraFront, radius);
	DrawDebugCircle(center, cameraFront, cameraUp, radius);
}

static void DrawDebugCapsule(glm::vec3 center, glm::quat rotation, float halfHeight, float radius, glm::vec3 cameraFront, glm::vec3 cameraUp, glm::vec3 cameraRight) {
	glm::vec4 colour = { 0.0f, 0.0f, 1.0f, 1.0f };
	glm::vec3 up = rotation * glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 right = rotation * glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 forward = rotation * glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 top = center + up * halfHeight;
	glm::vec3 bottom = center - up * halfHeight;

	IcePickRenderer::LinePointVertex3D points[8];
	points[0] = { top + right * radius, colour };
	points[1] = { bottom + right * radius, colour };
	points[2] = { top - right * radius, colour };
	points[3] = { bottom - right * radius, colour };

	points[4] = { top + forward * radius, colour };
	points[5] = { bottom + forward * radius, colour };
	points[6] = { top - forward * radius, colour };
	points[7] = { bottom - forward * radius, colour };

	DrawLine(points[0], points[1]);
	DrawLine(points[2], points[3]);
	DrawLine(points[4], points[5]);
	DrawLine(points[6], points[7]);
	DrawDebugSphere(top, radius, cameraFront, cameraUp, cameraRight);
	DrawDebugSphere(bottom, radius, cameraFront, cameraUp, cameraRight);
}

Viewport::Viewport(IcePick::EngineAPI engineAPI) :
	m_EngineAPI(engineAPI)
{
	m_ViewportSize = ImVec2(1920, 180);
	m_UsingGizmo = false;
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

	glm::vec2 mouseDelta = (m_ViewportRightClicked) ? m_EngineAPI.GetMouseDelta() : glm::vec2(0.0f, 0.0f);
	if (m_LockCursorFirstFrame) {
		mouseDelta = glm::vec2(0.0f, 0.0f);
		m_LockCursorFirstFrame = false;
	}

	auto& activeSceneRegistry = IcePick::GetActiveSceneRegistry();
	auto sceneCameraView = activeSceneRegistry.view<IcePick::SceneCamera>();

	for (auto entity : sceneCameraView) {
		IcePick::SceneCamera& sceneCamera = IcePick::GetComponent<IcePick::SceneCamera>(entity);
		sceneCamera.aspectRatio = m_ViewportSize.x / (float)m_ViewportSize.y;
	}

	m_EditorCamera.aspectRatio = m_ViewportSize.x / (float)m_ViewportSize.y;
	m_EditorCamera.yaw += mouseDelta.x * 0.2;
	m_EditorCamera.pitch += mouseDelta.y * 0.152;
	m_EditorCamera.OnUpdate(dt);
	glm::mat4 viewProjMatrix = m_EditorCamera.GetViewProjectionMatrix();
	IcePickRenderer::SetRenderViewProjectionMatrix(viewProjMatrix);
	IcePickRenderer::SetRenderCameraWorldPosition(m_EditorCamera.cameraPosition);
}

void Viewport::OnViewportEvent(IcePick::Event& event) {
	keyState.OnEvent(event);

	if (m_GameIsFocused && keyState.IsKeyPressed(IcePick::IP_KEY_ESC)) {
		IcePickRenderer::RequestCursorUnlock();
		m_GameIsFocused = false;
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

	if (!m_GameIsFocused) {
		event.flags |= IP_EVENT_HANDLED;
	}
}

void Viewport::Render(unsigned int renderTexture) {
	IP_CORE_PROFILE_BEGIN("Viewport render");
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin(m_ID);

	m_WindowPosition = ImGui::GetWindowPos();
	m_ViewportSize = ImGui::GetContentRegionAvail();
	ImVec2 regionMin = ImGui::GetWindowContentRegionMin();
	m_WindowPosition = ImVec2(m_WindowPosition.x + regionMin.x, m_WindowPosition.y + regionMin.y);
	ImVec2 mousePos = ImGui::GetMousePos();
	m_WindowMousePosition = ImVec2(mousePos.x - m_WindowPosition.x, mousePos.y - m_WindowPosition.y);

	ImGui::Image((void*)(intptr_t)renderTexture, m_ViewportSize, ImVec2(0, 1), ImVec2(1, 0));
	
	if (ImGui::BeginDragDropTarget()) {
		if (ImGui::AcceptDragDropPayload("MATERIAL_INSTANCE_ASSET")) {
			DropMaterialIntoViewport();
		}
		ImGui::EndDragDropTarget();
	}

	RenderViewportControls();

	if ((m_SelectedEntity != entt::null) && !m_GameIsPlaying) {
		RenderEntityGizmos();
		RenderRigidBodyDebugColliders();
	}

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
		if (m_GameIsPlaying && !m_GameIsFocused) {
			IcePickRenderer::RequestCursorLock();
			m_GameIsFocused = true;
		}

		if (!m_GameIsPlaying && !m_UsingGizmo) {
			uint32_t pixelData[2] = { 0, 0 };
			GetViewportDebugData(pixelData);
			m_EntitySelected = true;
			m_SelectedEntity = (entt::entity)pixelData[0];
		}
	}

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered() && !m_GameIsPlaying) {
		IcePickRenderer::RequestCursorLock();
		m_ViewportRightClicked = true;
		m_LockCursorFirstFrame = true;
	}

	if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && !m_GameIsPlaying) {
		IcePickRenderer::RequestCursorUnlock();
		m_ViewportRightClicked = false;
		m_LockCursorFirstFrame = false;
	}

	ImGui::End();
	ImGui::PopStyleVar();
	IP_CORE_PROFILE_POP();
}

void Viewport::RenderEntityGizmos() {
	if (!IcePick::HasComponent<IcePick::TransformComponent>(m_SelectedEntity))
		return;

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();
	
	ImGuizmo::SetRect(m_WindowPosition.x, m_WindowPosition.y, m_ViewportSize.x, m_ViewportSize.y);

	IcePick::TransformComponent* entityTransform = &IcePick::GetComponent<IcePick::TransformComponent>(m_SelectedEntity);
	glm::mat4 entityTransformMatrix = glm::mat4(1.0f);
	entityTransformMatrix = glm::translate(entityTransformMatrix, entityTransform->Position);
	entityTransformMatrix *= glm::toMat4(entityTransform->Rotation);
	entityTransformMatrix = glm::scale(entityTransformMatrix, entityTransform->Scale);

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
		using namespace IcePick;
		if ((keyState.IsKeyHeld(IP_KEY_LEFT_SHIFT) || keyState.IsKeyHeld(IP_KEY_RIGHT_SHIFT)) && !m_UsingGizmo) { // first frame using the gizmo when holding Shift
			m_SelectedEntity = DuplicateEntity<
				TagComponent,
				TransformComponent,
				MeshRendererComponent,
				ScriptComponent,
				RigidBodyComponent>(m_SelectedEntity);
			m_EntitySelected = true;
			entityTransform = &GetComponent<TransformComponent>(m_SelectedEntity);
		}

		m_UsingGizmo = true;
		glm::vec3 translation, scale, skew;
		glm::vec4 perspective;
		glm::quat rotation;

		glm::decompose(entityTransformMatrix, scale, rotation, translation, skew, perspective);

		entityTransform->Position = translation;
		entityTransform->Scale = scale;
		entityTransform->Rotation = rotation;
	}
	else {
		m_UsingGizmo = false;
	}

}

void Viewport::RenderRigidBodyDebugColliders() {
	bool drawDebugColliders = m_EngineAPI.QueryEngineRenderDebugPhysics();
	if (!drawDebugColliders)
		return;

	if (!IcePick::HasComponent<IcePick::RigidBodyComponent>(m_SelectedEntity))
		return;

	IcePick::RigidBodyComponent& rigidBody = IcePick::GetComponent<IcePick::RigidBodyComponent>(m_SelectedEntity);
	if (rigidBody.ColliderShapeCount == 0)
		return;

	glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraFront = glm::normalize(m_EditorCamera.cameraFront);
	glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

	IcePick::TransformComponent& entityTransform = IcePick::GetComponent<IcePick::TransformComponent>(m_SelectedEntity);
	for (unsigned int i = 0; i < rigidBody.ColliderShapeCount; i++) {
		IcePick::ColliderShape& collider = rigidBody.ColliderShapes[i];
		glm::vec3 colliderCenter = entityTransform.Position + entityTransform.Rotation * collider.ColliderOffset;

		switch (collider.ShapeType) {
		case IcePick::ColliderShape::ColliderShapeType::BOX_SHAPE:
		{
			DrawDebugBox(colliderCenter, entityTransform.Rotation, collider.ColliderScale * entityTransform.Scale);
			break;
		}
		case IcePick::ColliderShape::ColliderShapeType::SPHERE_SHAPE:
		{
			float maxComponent = std::max({ entityTransform.Scale.x, entityTransform.Scale.y, entityTransform.Scale.z });
			DrawDebugSphere(colliderCenter, collider.Radius * maxComponent, cameraFront, cameraUp, cameraRight);
			break;
		}
		case IcePick::ColliderShape::ColliderShapeType::CAPSULE_SHAPE:
		{
			float maxComponent = std::max({ entityTransform.Scale.x, entityTransform.Scale.y, entityTransform.Scale.z });
			DrawDebugCapsule(colliderCenter, entityTransform.Rotation, collider.ColliderScale.y * maxComponent, collider.Radius * maxComponent, cameraFront, cameraUp, cameraRight);
			break;
		}
		default:
			break;
		}
	}
}

void Viewport::RenderViewportControls() {
	ImVec2 buttonSize{ 40, 40 };
	ImVec2 viewportControlPosition{ (m_ViewportSize.x / 2) - (buttonSize.x / 2), 26};

	ImGui::SetCursorPos(viewportControlPosition);

	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(45, 45, 45, 120));

	IcePick::RuntimeState currentEngineRuntimeState = m_EngineAPI.QueryEngineRuntimeState();
	switch (currentEngineRuntimeState) {

	case IcePick::RuntimeState::STOPPED:
		if (ImGui::Button(ICON_FA_PLAY, buttonSize)) {
			m_EngineAPI.SetEngineRuntimeState(IcePick::RuntimeState::RUNNING);

			entt::registry& editorRegistry = IcePick::GetSceneRegistry(IcePick::SceneRegistryTypes::DEFAULT);
			entt::registry& runtimeRegistry = IcePick::GetSceneRegistry(IcePick::SceneRegistryTypes::TEMPORARY);
			IcePick::DuplicateSceneRegistry(editorRegistry, runtimeRegistry);

			IcePick::SetActiveSceneRegistry(IcePick::SceneRegistryTypes::TEMPORARY);
			m_GameIsPlaying = true;
		}
		break;

	case IcePick::RuntimeState::PAUSED:
		break;

	case IcePick::RuntimeState::RUNNING:
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		if (ImGui::Button(ICON_FA_STOP, buttonSize)) {
			m_EngineAPI.SetEngineRuntimeState(IcePick::RuntimeState::STOPPED);
			entt::registry& runtimeRegistry = IcePick::GetSceneRegistry(IcePick::SceneRegistryTypes::TEMPORARY);
			runtimeRegistry.clear();

			IcePick::SetActiveSceneRegistry(IcePick::SceneRegistryTypes::DEFAULT);
			IcePickRenderer::RequestCursorUnlock();
			m_GameIsPlaying = false;

			entt::registry& activeRegistry = IcePick::GetActiveSceneRegistry();
			if (!activeRegistry.valid(m_SelectedEntity)) {
				m_SelectedEntity = entt::null;
				m_EntitySelected = true;
			}
		}
		ImGui::PopStyleColor();
		break;
	default:
		break;
	}
	
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}

void Viewport::GetViewportDebugData(uint32_t* debugData) {
	int renderBufferWidth, renderBufferHeight;
	m_EngineAPI.GetRendererWindowSize(renderBufferWidth, renderBufferHeight);

	float xViewportRatio = m_WindowMousePosition.x / (float)m_ViewportSize.x;
	float yViewportRatio = 1.0f - (m_WindowMousePosition.y / (float)m_ViewportSize.y);

	int x = renderBufferWidth * xViewportRatio;
	int y = renderBufferHeight * yViewportRatio;

	m_EngineAPI.GetEntityMatPixelData(x, y, debugData);
}

void Viewport::DropMaterialIntoViewport() {
	uint32_t pixelData[2] = { 0, 0 };
	GetViewportDebugData(pixelData);
	
	entt::entity viewEntity = (entt::entity)pixelData[0];
	uint32_t materialSlot = pixelData[1];

	if (viewEntity == entt::null)
		return;

	if (!IcePick::HasComponent<IcePick::MeshRendererComponent>(viewEntity)) // Has another type of renderer which can be mouse selected, but no materials apply
		return;

	IcePick::MeshRendererComponent& entityMeshRenderer = IcePick::GetComponent<IcePick::MeshRendererComponent>(viewEntity);

	IcePick::UUID droppedMaterialInstanceId = m_EngineAPI.LoadMaterialInstanceFromAsset(m_DropAssetPath);
	entityMeshRenderer.MaterialSlots[materialSlot] = droppedMaterialInstanceId;
}

Viewport::~Viewport() {

}

