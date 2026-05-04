#pragma once
#include "Public/EngineAPI.h"

class Toolbar {
public:
	Toolbar() = delete;
	Toolbar(IcePick::EngineAPI engineAPI);
	void Render();
private:
	const char* m_Title = "Toolbar";
	IcePick::EngineAPI m_EngineAPI;
};