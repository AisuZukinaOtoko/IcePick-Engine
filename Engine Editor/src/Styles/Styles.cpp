#include "Utilities/Assert.h"
#include "LogSystem.h"
#include "Styles.h"
#include <IconsFontAwesome7.h>

void Styles::Init(IcePick::EngineAPI& engineAPI) {
	m_IconTextures[ICON_GENERIC_FILE] = engineAPI.GetTexture("Engine Editor/res/Textures/icons/file_icon.png");
	m_IconTextures[ICON_FOLDER] = engineAPI.GetTexture("Engine Editor/res/Textures/icons/folder_icon.png");
	m_IconTextures[ICON_FBX_FILE] = engineAPI.GetTexture("Engine Editor/res/Textures/icons/fbx_icon.png");
	m_IconTextures[ICON_GLB_FILE] = engineAPI.GetTexture("Engine Editor/res/Textures/icons/glb_icon.png");
	m_IconTextures[ICON_OBJ_FILE] = engineAPI.GetTexture("Engine Editor/res/Textures/icons/obj_icon.png");
	m_IconTextures[ICON_STATIC_MESH_ASSET] = engineAPI.GetTexture("Engine Editor/res/Textures/icons/cube_icon.png");
	m_IconTextures[ICON_MATERIAL_ASSET] = engineAPI.GetTexture("Engine Editor/res/Textures/icons/material_icon.png");
	m_IconTextures[ICON_SCRIPT_ASSET] = engineAPI.GetTexture("Engine Editor/res/Textures/icons/lua_icon.png");
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImColor(45, 45, 45, 255);
	//style.Colors[ImGuiCol_Border] = ImColor(200, 200, 200, 0);
	//style.Colors[ImGuiCol_WindowBg] = ImColor(22, 22, 22, 255);
	style.Colors[ImGuiCol_ChildBg] = ImColor(45, 45, 45, 255);
	//style.Colors[ImGuiCol_ChildBg] = ImColor(22, 22, 22, 255);
	style.Colors[ImGuiCol_PopupBg] = ImColor(45, 45, 45, 255);
	style.Colors[ImGuiCol_FrameBg] = ImColor(15, 15, 15, 255);
	//style.Colors[ImGuiCol_FrameBg] = ImColor(110, 110, 110, 255);
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
	style.Colors[ImGuiCol_TabSelectedOverline] = ImColor(0, 140, 140, 255);
	style.Colors[ImGuiCol_TabDimmed] = ImColor(40, 40, 40, 255);
	style.Colors[ImGuiCol_TabDimmedSelected] = ImColor(0, 140, 140, 255);
	style.Colors[ImGuiCol_TabDimmedSelectedOverline] = ImColor(0, 140, 140, 255);
	style.Colors[ImGuiCol_DockingPreview] = ImColor(0, 200, 200, 255);
	style.Colors[ImGuiCol_DragDropTarget] = ImColor(0, 255, 255, 255);

	const float largeFontSize = 18.0f;
	const float smallFontSize = 13.0f;
	
	ImGuiIO& io = ImGui::GetIO();
	std::filesystem::path fontPath = "Engine Editor/res/Fonts/freesans-font/FreeSans-LrmZ.ttf";
	std::filesystem::path iconFontPath = "Engine Editor/res/Fonts/fontawesome-free-7.2.0-desktop/otfs/Font Awesome 7 Free-Solid-900.otf";
	static const ImWchar iconRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	
	// Large Font
	ImFontConfig largeFontConfig;
	largeFontConfig.PixelSnapH = true;
	largeFontConfig.MergeMode = true;
	m_LargeFont = io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), largeFontSize);
	largeFontConfig.GlyphOffset.y = 1.0f;
	io.Fonts->AddFontFromFileTTF(
		iconFontPath.string().c_str(), largeFontSize,
		&largeFontConfig, iconRanges
	);

	// Small Font
	ImFontConfig smallFontConfig;
	smallFontConfig.PixelSnapH = true;
	smallFontConfig.MergeMode = true;
	m_SmallFont = io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), smallFontSize);
	smallFontConfig.GlyphOffset.y = 1.0f;
	io.Fonts->AddFontFromFileTTF(
		iconFontPath.string().c_str(), smallFontSize,
		&smallFontConfig, iconRanges
	);

	io.Fonts->Build();
	if (!m_LargeFont)
		IP_LOG("Failed to load large editor font: " + fontPath.string(), IP_WARN_LOG);

	if (!m_SmallFont)
		IP_LOG("Failed to load small editor font: " + fontPath.string(), IP_WARN_LOG);
}

unsigned int Styles::GetIconTexture(Icon icon) const {
	IP_ASSERT(icon != ICON_COUNT, "Invalid icon.");
	return m_IconTextures[icon];
}
