#pragma once
#include "Public/EngineAPI.h"
#include <imgui-docking/imgui.h>

class Styles {
public:
	void Init(IcePick::EngineAPI& engineAPI);
	enum Icon {
		ICON_GENERIC_FILE,
		ICON_FOLDER,
		ICON_FBX_FILE,
		ICON_GLB_FILE,
		ICON_OBJ_FILE,
		ICON_STATIC_MESH_ASSET,
		ICON_MATERIAL_ASSET,
		ICON_SCRIPT_ASSET,
		ICON_COUNT
	};

	unsigned int GetIconTexture(Icon icon) const;

	ImFont* GetLargeFont() { return m_LargeFont; }
	ImFont* GetSmallFont() { return m_SmallFont; }
private:
	ImFont* m_LargeFont = nullptr;
	ImFont* m_SmallFont = nullptr;
	unsigned int m_IconTextures[ICON_COUNT];
};