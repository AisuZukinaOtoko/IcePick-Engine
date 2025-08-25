#pragma once
#include "../../src/Layers/EngineAPI.h"

class Styles {
public:
	void Init(IcePick::EngineAPI& engineAPI);
	enum Icon {
		ICON_GENERIC_FILE,
		ICON_FOLDER,
		ICON_FBX_FILE,
		ICON_GLB_FILE,
		ICON_OBJ_FILE,
		ICON_COUNT
	};

	unsigned int GetIconTexture(Icon icon);
private:
	unsigned int m_IconTextures[ICON_COUNT];
};