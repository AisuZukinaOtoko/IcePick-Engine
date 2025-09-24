#pragma once
#include "PanelCommon.h"
#include <string>
#include <filesystem>

class AssetBrowser {
public:
	AssetBrowser() = delete;
	AssetBrowser(IcePick::EngineAPI engineAPI);
	void Init(IcePick::EngineAPI& engineAPI, Styles styles);
	void Render();
	std::string GetDragFilePath();
private:
	void* GetFileIcon(std::filesystem::path extension);
	Styles m_Styles;
	const char* m_Title = "Asset Browser";
	std::string m_DragFilePath;
	IcePick::EngineAPI m_EngineAPI;
	std::filesystem::path m_CurrentBrowsingPath;
};