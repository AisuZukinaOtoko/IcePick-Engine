#include "MeshImportPopup.h"
#include <imgui-docking/imgui.h>

static constexpr unsigned int TextBufferSize = 255;
static char InputTextBuffer[TextBufferSize];

void MeshImportPopup::OpenPopup() {
	ImGui::OpenPopup(m_Title);
	m_Open = true;
}

void MeshImportPopup::Render() {
	if (!m_Open)
		return;

    if (ImGui::BeginPopupModal(m_Title)) {
        ImGui::Checkbox("Import Meshes", &m_ImportSettings.LoadMesh);
        ImGui::Checkbox("Import Materials", &m_ImportSettings.LoadMaterials);

        if (ImGui::Checkbox("Import Skeleton", &m_ImportSettings.LoadSkeleton)) {
            m_ImportSettings.LoadMeshAs = (m_ImportSettings.LoadSkeleton) ? IcePick::ImportSettings::MeshType::SKELETAL_MESH : IcePick::ImportSettings::MeshType::STATIC_MESH;
        }

        ImGui::Checkbox("Import Animations", &m_ImportSettings.LoadAnimations);
        //ImGui::InputText("File name (.ipmtb)", InputTextBuffer, sizeof(InputTextBuffer));

        if (ImGui::Button("Import", ImVec2(120, 0))) {         
            m_ImportSubmitted = true;
            ClosePopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ClosePopup();
        }
        ImGui::EndPopup();
    }

}

void MeshImportPopup::ClosePopup() {
    ImGui::CloseCurrentPopup();
    m_Open = false;
}