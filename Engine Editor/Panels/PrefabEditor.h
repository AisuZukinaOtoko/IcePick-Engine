#pragma once
#include "PanelCommon.h"
#include "../EditorRenderer.h"
#include <filesystem>

class PrefabEditor {
public:
	PrefabEditor(IcePick::EngineAPI engineAPI);
	void SetEditPrefab(entt::entity prefabId);
	void SetDropAssetPath(std::filesystem::path filePath);
	void Render(const Styles& styles);
	~PrefabEditor();
private:
	entt::entity m_EditPrefabId = entt::null;
	IcePick::EngineAPI m_EngineAPI;
	EditorRenderer m_PreviewRenderer;
	const char* m_Title = "Prefab Editor";
	std::filesystem::path m_DropAssetPath;
	bool m_Open = true;
	unsigned int m_PreviewViewportSizeX = 200;
	unsigned int m_PreviewViewportSizeY = 200;
};
