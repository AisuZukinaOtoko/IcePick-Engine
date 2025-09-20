#pragma once
#include "imgui-docking/imgui.h"
#include "imgui-docking/ImGuizmo.h"
#include "EditorCamera.h"
#include "../Event Systems/Event.h"
#include "../Vendor/entt/entt.h"
#include "../Layers/EngineAPI.h"
#include <functional>
#include <filesystem>

class Viewport {
public:
	Viewport() = delete;
	Viewport(IcePick::EngineAPI engineAPI);
	~Viewport();

	void SetSelectedEntityChangeCallback(std::function<void(entt::entity)> callback);
	void SetSelectedEntity(entt::entity entity);
	void SetDropAssetPath(std::string filePath);
	void OnUpdate(DeltaTime dt);
	void OnViewportEvent(IcePick::Event& event);
	void Render(unsigned int frameBuffer);

private:
	ImVec2 m_WindowPosition;
	ImVec2 m_WindowMousePosition;
	ImVec2 m_ViewportSize;
	ImVec2 m_MouseDelta;
	const char* m_ID = "Viewport";
	bool m_ViewportRightClicked = false;
	EditorCamera m_EditorCamera;
	IcePick::EngineAPI m_EngineAPI;

	std::filesystem::path m_DropAssetPath;
	std::function<void(entt::entity)> SelectedEntityChangeCallback;
	bool m_UsingGizmo = false;
	bool m_EntitySelected = false;
	entt::entity m_SelectedEntity = entt::null;
	ImGuizmo::OPERATION m_GizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	void RenderEntityGizmos();
	void GetViewportDebugData(uint32_t* debugData);
	void DropMaterialIntoViewport();
};