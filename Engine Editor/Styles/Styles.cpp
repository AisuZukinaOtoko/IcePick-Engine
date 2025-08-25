#include "../../src/Utilities/Assert.h"
#include "Styles.h"

void Styles::Init(IcePick::EngineAPI& engineAPI) {
	m_IconTextures[ICON_GENERIC_FILE] = engineAPI.NewTexture("res/textures/icons/file_icon.png");
	m_IconTextures[ICON_FOLDER] = engineAPI.NewTexture("res/textures/icons/folder_icon.png");
	m_IconTextures[ICON_FBX_FILE] = engineAPI.NewTexture("res/textures/icons/fbx_icon.png");
	m_IconTextures[ICON_GLB_FILE] = engineAPI.NewTexture("res/textures/icons/glb_icon.png");
	m_IconTextures[ICON_OBJ_FILE] = engineAPI.NewTexture("res/textures/icons/obj_icon.png");
}

unsigned int Styles::GetIconTexture(Icon icon) {
	IP_ASSERT(icon != ICON_COUNT, "Invalid icon.");
	return m_IconTextures[icon];
}
