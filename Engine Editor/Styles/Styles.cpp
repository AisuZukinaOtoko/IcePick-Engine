#include "../../src/Utilities/Assert.h"
#include "Styles.h"
#include "../Vendor/imgui-docking/imgui.h"

void Styles::Init(IcePick::EngineAPI& engineAPI) {
	m_IconTextures[ICON_GENERIC_FILE] = engineAPI.GetTexture("res/textures/icons/file_icon.png");
	m_IconTextures[ICON_FOLDER] = engineAPI.GetTexture("res/textures/icons/folder_icon.png");
	m_IconTextures[ICON_FBX_FILE] = engineAPI.GetTexture("res/textures/icons/fbx_icon.png");
	m_IconTextures[ICON_GLB_FILE] = engineAPI.GetTexture("res/textures/icons/glb_icon.png");
	m_IconTextures[ICON_OBJ_FILE] = engineAPI.GetTexture("res/textures/icons/obj_icon.png");
	m_IconTextures[ICON_STATIC_MESH_ASSET] = engineAPI.GetTexture("res/textures/icons/cube_icon.png");
	m_IconTextures[ICON_MATERIAL_ASSET] = engineAPI.GetTexture("res/textures/icons/globe_icon.png");
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImColor(22, 22, 22, 255);
	style.Colors[ImGuiCol_ChildBg] = ImColor(22, 22, 22, 255);
	style.Colors[ImGuiCol_PopupBg] = ImColor(22, 22, 22, 255);
	style.Colors[ImGuiCol_FrameBg] = ImColor(110, 110, 110, 255);
	style.Colors[ImGuiCol_FrameBgHovered] = ImColor(100, 100, 100, 255);
	style.Colors[ImGuiCol_FrameBgActive] = ImColor(80, 80, 80, 255);
	style.Colors[ImGuiCol_TitleBgActive] = ImColor(0, 0, 0, 255);
	style.Colors[ImGuiCol_CheckMark] = ImColor(255, 255, 255, 255);
	style.Colors[ImGuiCol_SliderGrab] = ImColor(200, 200, 200, 255);
	style.Colors[ImGuiCol_SliderGrabActive] = ImColor(220, 220, 220, 255);
	style.Colors[ImGuiCol_Button] = ImColor(100, 100, 100, 255);
	style.Colors[ImGuiCol_ButtonHovered] = ImColor(150, 150, 150, 255);
	style.Colors[ImGuiCol_ButtonActive] = ImColor(60, 60, 60, 255);
	style.Colors[ImGuiCol_Header] = ImColor(60, 60, 60, 255);
	style.Colors[ImGuiCol_HeaderHovered] = ImColor(90, 90, 90, 255);
	style.Colors[ImGuiCol_HeaderActive] = ImColor(45, 45, 45, 255);
	style.Colors[ImGuiCol_Separator] = ImColor(100, 100, 100, 255);
	style.Colors[ImGuiCol_SeparatorHovered] = ImColor(120, 120, 120, 255);
	style.Colors[ImGuiCol_SeparatorActive] = ImColor(90, 90, 90, 255);
	style.Colors[ImGuiCol_ResizeGrip] = ImColor(100, 100, 100, 255);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImColor(140, 140, 140, 255);
	style.Colors[ImGuiCol_ResizeGripActive] = ImColor(70, 70, 70, 255);
	style.Colors[ImGuiCol_Tab] = ImColor(110, 110, 110, 255);
	style.Colors[ImGuiCol_TabHovered] = ImColor(150, 150, 150, 255);
	style.Colors[ImGuiCol_TabSelected] = ImColor(0, 140, 140, 255);
	style.Colors[ImGuiCol_TabSelectedOverline] = ImColor(0, 0, 0, 0);
	style.Colors[ImGuiCol_TabDimmed] = ImColor(40, 40, 40, 255);
	style.Colors[ImGuiCol_TabDimmedSelected] = ImColor(0, 140, 140, 255);
	style.Colors[ImGuiCol_DockingPreview] = ImColor(0, 200, 200, 255);
	style.Colors[ImGuiCol_DragDropTarget] = ImColor(0, 255, 255, 255);
}

unsigned int Styles::GetIconTexture(Icon icon) const {
	IP_ASSERT(icon != ICON_COUNT, "Invalid icon.");
	return m_IconTextures[icon];
}
