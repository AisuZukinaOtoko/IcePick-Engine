#pragma once
#include "../Layers/EngineAPI.h"

class EditorRenderer {
public:
	EditorRenderer() = delete;
	EditorRenderer(IcePick::EngineAPI engineAPI);
	void Destroy();
	~EditorRenderer();
private:
	IcePick::EngineAPI m_EngineAPI;
};