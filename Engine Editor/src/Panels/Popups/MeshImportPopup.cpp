#include "MeshImportPopup.h"
#include <imgui-docking/imgui.h>

static constexpr size_t TextBufferSize = 40;
static char InputAssetNameBuffer[TextBufferSize];

void MeshImportPopup::OpenPopup(const std::filesystem::path& meshPath) {
	ImGui::OpenPopup(m_Title);
	m_Open = true;

    m_MeshParentDirectory = meshPath.parent_path();
    std::snprintf(InputAssetNameBuffer, TextBufferSize, meshPath.stem().string().c_str());
    m_ImportSettings.ImportAssetName = InputAssetNameBuffer;
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

        if (ImGui::BeginTable("TextureFormats", 2, ImGuiTableFlags_Borders)) {
            ImGui::TableNextRow(ImGuiTableRowFlags_None);
            ImGui::TableNextColumn();

            ImGui::Text("Albedo Texture is sRGB");
            ImGui::TableNextColumn();
            ImGui::Checkbox("##Albedo", &m_ImportSettings.LoadDiffuseTextureAsSRGB);

            ImGui::EndTable();
        }

        if (ImGui::InputText("Asset Name", InputAssetNameBuffer, sizeof(InputAssetNameBuffer))) {
            m_ImportSettings.ImportAssetName = InputAssetNameBuffer;
        }

        if (ImGui::Button("Import", ImVec2(120, 0))) {         
            m_ImportSubmitted = true;
            m_ImportSettings.ImportTargetLocation = m_MeshParentDirectory / m_ImportSettings.ImportAssetName;
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