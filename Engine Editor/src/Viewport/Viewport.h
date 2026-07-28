#pragma once
#include <imgui-docking/imgui.h>
#include <imgui-docking/ImGuizmo.h>
#include <entt/entt.h>
#include "EditorCamera.h"
#include "SelectionContext.h"
#include "Event Systems/Event.h"
#include "Public/EngineAPI.h"
#include <functional>
#include <filesystem>

class Viewport {
public:
	Viewport() = delete;
	Viewport(IcePick::EngineAPI engineAPI);
	~Viewport();

	void SetSelectionContextChangeCallback(std::function<void(SelectionContext)> callback);
	void SetSelectionContext(SelectionContext selectionContext);

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

	bool m_GameIsPlaying = false;
	bool m_GameIsFocused = false; // Do not process game events from the editor

	// To disable mouse deltas for the first frame after locking the cursor.
	// GLFW moves the cursor to the center of the screen, which messes with mouse deltas.
	// As a result, the viewport camera will not move on the first frame
	bool m_LockCursorFirstFrame = false;

	EditorCamera m_EditorCamera;
	IcePick::EngineAPI m_EngineAPI;

	std::filesystem::path m_DropAssetPath;
	std::function<void(SelectionContext)> SelectionContextChangeCallback;
	bool m_UsingGizmo = false;

	bool m_SelectionContextChanged = false;
	SelectionContext m_SelectionContext;

	ImGuizmo::OPERATION m_GizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE m_GizmoCoordinateSpace = ImGuizmo::MODE::LOCAL;
	void RenderEntityGizmos();
	void RenderSkeletonGizmos();
	void RenderRigidBodyDebugColliders();
	void RenderViewportControls();
	void GetViewportDebugData(uint32_t* debugData);
	void DropMaterialIntoViewport();
};