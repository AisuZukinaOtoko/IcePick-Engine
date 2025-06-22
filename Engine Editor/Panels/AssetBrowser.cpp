#include "AssetBrowser.h"
#include <filesystem>

AssetBrowser::AssetBrowser() {
    m_CurrentBrowsingPath = "res/assets";
}

void AssetBrowser::Init(IcePick::EngineAPI& engineAPI) {
    engineAPI.SayHello();
}

void AssetBrowser::Render() {
	ImGui::Begin(m_Title);
	ImGui::Text("This is the asset browser.");

    if (ImGui::Button("Back")) {
        IP_LOG(m_CurrentBrowsingPath.string().c_str());
        m_CurrentBrowsingPath = m_CurrentBrowsingPath.parent_path();
        IP_LOG(m_CurrentBrowsingPath.string().c_str());
    }
    ImGui::Separator();

    // Configuration
    const float iconSize = 120.0f;
    //const float iconSize = 90.0f;
    const float padding = 0.0f;
    const float cellSize = iconSize + padding;
    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1) columnCount = 1;

    // Create the grid
    ImGui::Columns(columnCount, nullptr, false);

    int tempIterator = 0;
    for (const auto& file : std::filesystem::directory_iterator(m_CurrentBrowsingPath)) {
        tempIterator++;
        ImTextureID icon = (void*)1;

        if (file.is_regular_file()) {
            std::filesystem::path extension = file.path().extension();

            if (extension == ".fbx") {
                icon = (void*)2;
            }
            else if (extension == ".glb") {
                icon = (void*)3;
            }
            else if (extension == ".obj") {
                icon = (void*)4;
            }
            else {
                icon = (void*)5;
            }
        }
        else if (file.is_directory()) {

        }
        

        ImGui::PushID(tempIterator);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));
        ImGui::ImageButton("##Hello", icon, ImVec2(iconSize, iconSize), ImVec2(0, 1), ImVec2(1, 0), ImVec4(0, 0, 0, 1));
        ImGui::PopStyleVar(3);

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
            // Open asset or navigate into folder
            if (file.is_directory()) {
                m_CurrentBrowsingPath = file.path();
            }
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            std::filesystem::path assetPath = file.path();
            // Set the payload — it can be anything (structs, pointers, etc.)
            m_DragFilePath = assetPath.string();
            ImGui::SetDragDropPayload("ASSET", nullptr, 0, ImGuiCond_Once);

            // Optionally show preview while dragging
            ImGui::ImageButton("##Hello", icon, ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0), ImVec4(0, 0, 0, 1));
            ImGui::SameLine();
            ImGui::Text(file.path().filename().string().c_str());
            ImGui::EndDragDropSource();
        }

        //ImGui::TextWrapped(file.path().string().c_str());
        ImGui::TextWrapped(file.path().filename().string().c_str());

        ImGui::NextColumn();
        ImGui::PopID();
    }

    ImGui::Columns(1);
	ImGui::End();
}

std::string AssetBrowser::GetDragFilePath() {
    return m_DragFilePath;
}
