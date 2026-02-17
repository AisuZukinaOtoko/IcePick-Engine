#pragma once
#include "PanelCommon.h"
#include "../Vendor/entt/entt.h"
#include <functional>

class ScenePanel {
public:
	ScenePanel();
	~ScenePanel();
	void SetSelectedEntityChangeCallback(std::function<void(entt::entity)> callback);
	void SetSelectedEntity(entt::entity entity);
	void OnUpdate(DeltaTime dt);
	void ShowSceneHierarchy();
	bool EntitySelected();

	entt::entity GetDraggedEntity();
	entt::entity GetSelectedEntity();
private:
	std::function<void(entt::entity)> SelectedEntityChangeCallback;
	const char* m_Title;
	entt::entity m_DraggedEntity = entt::null;
	entt::entity m_SelectedEntity = entt::null;
	bool m_EntitySelected = false;
	bool m_IsDraggingItem = false;
};