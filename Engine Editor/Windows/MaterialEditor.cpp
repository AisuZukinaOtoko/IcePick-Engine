#include "MaterialEditor.h"
#include "imgui-docking/imgui.h"
#include "../LogSystem.h"

MaterialEditor::MaterialEditor(IcePick::EngineAPI engineAPI) :
	m_EngineAPI(engineAPI),
	m_Renderer(engineAPI)
{

}

void MaterialEditor::SetEditMaterial(IcePick::UUID materialID) {
	m_Open = true;
	m_EditMaterialId = materialID;
}

void MaterialEditor::Render() {
	if (!m_Open) {
		return;
	}

	ImGui::Begin(m_ID, &m_Open, ImGuiWindowFlags_NoCollapse);
	ImGui::Text("This is the material editor!!!! :)");
	ImGui::End();
}

MaterialEditor::~MaterialEditor() {

}
