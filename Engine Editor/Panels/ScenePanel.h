#pragma once
#include "PanelCommon.h"

class ScenePanel {
public:
	ScenePanel();
	~ScenePanel();
	void ShowSceneHierarchy();
	bool EntitySelected();
	entt::entity GetSelectedEntity();
private:
	const char* m_Title;
	entt::entity m_SelectedEntity = entt::null;
	bool m_EntitySelected = false;
};