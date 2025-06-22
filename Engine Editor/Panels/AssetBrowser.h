#pragma once
#include "PanelCommon.h"
#include <string>
#include <filesystem>

class AssetBrowser {
public:
	AssetBrowser();
	void Init(IcePick::EngineAPI& engineAPI);
	void Render();
	std::string GetDragFilePath();
private:
	const char* m_Title = "Asset Browser";
	std::string m_DragFilePath;
	std::filesystem::path m_CurrentBrowsingPath;
};