#include "DopeSheet.h"
#include <imgui-docking/imgui.h>
#include <IconsFontAwesome7.h>

DopeSheet::DopeSheet() {
	m_Open = true;
}

void DopeSheet::OnUpdate(DeltaTime dt) {

}

void DopeSheet::Render() {
	if (!m_Open)
		return;

	ImGui::Begin(ICON_FA_FILM " Timeline", &m_Open, ImGuiWindowFlags_NoCollapse);

	ImGui::End();
}