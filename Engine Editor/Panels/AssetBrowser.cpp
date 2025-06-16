#include "AssetBrowser.h"
#include <filesystem>

void AssetBrowser::Render() {
	ImGui::Begin(m_Title);
	ImGui::Text("This is the asset browser.");
    // Configuration
    const float iconSize = 90.0f;
    const float padding = 0.0f;
    const float cellSize = iconSize + padding;
    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = (int)(panelWidth / cellSize);
    if (columnCount < 1) columnCount = 1;

    // Create the grid
    ImGui::Columns(columnCount, nullptr, false);
    IcePick::Entity tempEntity;

    //for (int i = 0; i < 30; i++) {
    //    // Your texture ID here (e.g. from OpenGL or your renderer)
    //    ImTextureID icon = (void*)1;

    //    ImGui::PushID(i);
    //    ImGui::ImageButton("##Hello",icon, ImVec2(iconSize, iconSize), ImVec2(0, 1), ImVec2(1, 0));

    //    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
    //        // Open asset or navigate into folder
    //    }

    //    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
    //        // Set the payload — it can be anything (structs, pointers, etc.)
    //        ImGui::SetDragDropPayload("ASSET", &tempEntity, sizeof(IcePick::Entity), ImGuiCond_Once);

    //        // Optionally show preview while dragging
    //        ImGui::ImageButton("##Hello", icon, ImVec2(iconSize, iconSize), ImVec2(0, 1), ImVec2(1, 0));
    //        ImGui::Text("Dragging: %s.", "some asset dude");
    //        ImGui::EndDragDropSource();
    //    }

    //    ImGui::TextWrapped("This is an asset");
    //    ImGui::NextColumn();
    //    ImGui::PopID();
    //}

    int tempIterator = 0;
    for (const auto& file : std::filesystem::directory_iterator("res/assets/")) {
        tempIterator++;
        if (file.is_regular_file()) {
            //std::cout << file.path() << "\n";
        }

        // Your texture ID here (e.g. from OpenGL or your renderer)
        ImTextureID icon = (void*)1;

        ImGui::PushID(tempIterator);
        ImGui::ImageButton("##Hello", icon, ImVec2(iconSize, iconSize), ImVec2(0, 1), ImVec2(1, 0));

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
            // Open asset or navigate into folder
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            std::filesystem::path assetPath = file.path();
            // Set the payload — it can be anything (structs, pointers, etc.)
            m_DragFilePath = assetPath.string();
            ImGui::SetDragDropPayload("ASSET", nullptr, 0, ImGuiCond_Once);
            //ImGui::SetDragDropPayload("ASSET", &assetPath, sizeof(std::filesystem::path), ImGuiCond_Once);

            // Optionally show preview while dragging
            ImGui::ImageButton("##Hello", icon, ImVec2(iconSize, iconSize), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::Text("Dragging: %s.", "some asset dude");
            ImGui::EndDragDropSource();
        }

        if (file.is_regular_file()) {
            //std::cout << file.path() << "\n";
            ImGui::TextWrapped(file.path().string().c_str());
        }
        ImGui::NextColumn();
        ImGui::PopID();
    }

    ImGui::Columns(1);
	ImGui::End();
}

std::string AssetBrowser::GetDragFilePath() {
    return m_DragFilePath;
}
