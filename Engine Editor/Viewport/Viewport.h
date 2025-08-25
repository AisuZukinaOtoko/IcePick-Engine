#pragma once
#include "imgui-docking/imgui.h"
#include "EditorCamera.h"
#include "../Event Systems/Event.h"

class Viewport {
public:
	Viewport();
	~Viewport();

	void OnUpdate(DeltaTime dt);
	void OnViewportEvent(IcePick::Event& event);
	void Render(unsigned int frameBuffer);

private:
	ImVec2 m_ViewportSize;
	ImVec2 m_MouseDelta;
	const char* m_ID = "Viewport";
	bool m_ViewportRightClicked = false;
	EditorCamera m_EditorCamera;
};