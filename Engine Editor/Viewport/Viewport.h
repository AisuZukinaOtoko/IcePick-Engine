#pragma once
#include "imgui-docking/imgui.h"
#include "EditorCamera.h"
#include "../Event Systems/Event.h"
#include "../Vendor/entt/entt.h"
#include <functional>

class Viewport {
public:
	Viewport();
	~Viewport();

	void SetSelectedEntityChangeCallback(std::function<void(entt::entity)> callback);
	void SetSelectedEntity(entt::entity entity);
	void OnUpdate(DeltaTime dt);
	void OnViewportEvent(IcePick::Event& event);
	void Render(unsigned int frameBuffer);

private:
	ImVec2 m_WindowPosition;
	ImVec2 m_ViewportSize;
	ImVec2 m_MouseDelta;
	const char* m_ID = "Viewport";
	bool m_ViewportRightClicked = false;
	EditorCamera m_EditorCamera;

	std::function<void(entt::entity)> SelectedEntityChangeCallback;
	bool m_EntitySelected = false;
	entt::entity m_SelectedEntity = entt::null;
	void RenderEntityGuizmos();
};