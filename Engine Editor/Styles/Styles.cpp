#include "../../src/Utilities/Assert.h"
#include "Styles.h"

void Styles::Init(IcePick::EngineAPI& engineAPI) {
	m_IconTextures[ICON_GENERIC_FILE] = engineAPI.GetTexture("res/textures/icons/file_icon.png");
	m_IconTextures[ICON_FOLDER] = engineAPI.GetTexture("res/textures/icons/folder_icon.png");
	m_IconTextures[ICON_FBX_FILE] = engineAPI.GetTexture("res/textures/icons/fbx_icon.png");
	m_IconTextures[ICON_GLB_FILE] = engineAPI.GetTexture("res/textures/icons/glb_icon.png");
	m_IconTextures[ICON_OBJ_FILE] = engineAPI.GetTexture("res/textures/icons/obj_icon.png");
	m_IconTextures[ICON_STATIC_MESH_ASSET] = engineAPI.GetTexture("res/textures/icons/cube_icon.png");
	m_IconTextures[ICON_MATERIAL_ASSET] = engineAPI.GetTexture("res/textures/icons/globe_icon.png");
}

unsigned int Styles::GetIconTexture(Icon icon) const {
	IP_ASSERT(icon != ICON_COUNT, "Invalid icon.");
	return m_IconTextures[icon];
}
