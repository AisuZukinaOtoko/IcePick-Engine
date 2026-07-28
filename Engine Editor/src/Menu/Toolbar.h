#pragma once
#include "Public/EngineAPI.h"
#include "AssetRegistryViewer.h"

class Toolbar {
public:
	Toolbar() = delete;
	Toolbar(IcePick::EngineAPI engineAPI);
	void Render();
private:
	const char* m_Title = "Toolbar";
	IcePick::EngineAPI m_EngineAPI;
	AssetRegistryViewer m_AssetRegistryViewer;
};