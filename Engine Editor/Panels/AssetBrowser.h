#pragma once
#include "PanelCommon.h"
#include <string>

class AssetBrowser {
public:
	void Render();
	std::string GetDragFilePath();
private:
	const char* m_Title = "Asset Browser";
	std::string m_DragFilePath;
};