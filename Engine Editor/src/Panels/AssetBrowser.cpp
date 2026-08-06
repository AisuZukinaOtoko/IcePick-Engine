#include "AssetBrowser.h"
#include "File Systems/AssetTypes.h"
#include <filesystem>
#include "../Utils/Serialize.h"
#include "Utilities/DebugStatistics.h"
#include "LogSystem.h"
#include <IconsFontAwesome7.h>

static constexpr unsigned int bufferSize = 255;
static char TextInputBuffer[bufferSize];

static void ClearTextInputBuffer() {
    for (int i = 0; i < bufferSize; i++) {
        TextInputBuffer[i] = '\0';
    }
}

AssetBrowser::AssetBrowser(IcePick::EngineAPI engineAPI) :
    m_EngineAPI(engineAPI)
{
    ClearTextInputBuffer();
    m_CurrentBrowsingPath = std::filesystem::canonical("Game Engine/res/Assets");
    m_ProjectDirectory = std::filesystem::canonical("Game Engine/res");
}

void AssetBrowser::Init(IcePick::EngineAPI& engineAPI, Styles styles) {
    m_Styles = styles;
}

void AssetBrowser::SetEditMaterialCallback(std::function<void(std::filesystem::path)> callback) {
    EditMaterialCallback = callback;
}

void AssetBrowser::Render() {
    IP_CORE_PROFILE_BEGIN("Asset browser render");
	ImGui::Begin(m_Title);

    if (ImGui::BeginPopup("NEW_ASSET")) {
        bool closeMainPopUp = false;
        ImGui::Text("Select an Asset Type.");
        if (ImGui::Button("Material Base")) {                                                       
            ImGui::OpenPopup("Create Base Material");     
        }

        if (ImGui::Button("Script")) {
            ImGui::OpenPopup("Create New Script");
        }

        if (ImGui::BeginPopupModal("Create Base Material")) {
            ImGui::InputText("File name (.ipmtb)", TextInputBuffer, sizeof(TextInputBuffer));

            if (ImGui::Button("Save", ImVec2(120, 0))) {
                IcePick::MaterialBase newMaterialBase;
                newMaterialBase.ShaderId = IcePick::UUID{};
                IcePick::ShaderSource newMaterialShaderSource;
                Graph newMaterialShaderGraph;

                std::string fileName = std::string(TextInputBuffer) + ".ipmtb";
                std::filesystem::path newMaterialBasePath = m_CurrentBrowsingPath / fileName;
                SerializeMaterialBase(newMaterialBasePath, newMaterialBase, newMaterialShaderGraph, newMaterialShaderSource);

                ClearTextInputBuffer();
                ImGui::CloseCurrentPopup();
                closeMainPopUp = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                ClearTextInputBuffer();
                ImGui::CloseCurrentPopup();
                closeMainPopUp = true;
            }
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopupModal("Create New Script")) {
            ImGui::InputText("File name (.lua)", TextInputBuffer, sizeof(TextInputBuffer));

            if (ImGui::Button("Save", ImVec2(120, 0))) {

                std::string fileName = std::string(TextInputBuffer) + ".lua";
                std::filesystem::path newScriptPath = m_CurrentBrowsingPath / fileName;
                CreateNewScriptTemplate(newScriptPath);

                ClearTextInputBuffer();
                ImGui::CloseCurrentPopup();
                closeMainPopUp = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                ClearTextInputBuffer();
                ImGui::CloseCurrentPopup();
                closeMainPopUp = true;
            }
            ImGui::EndPopup();
        }

        if (closeMainPopUp) {
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }

    ImGui::Separator();

    ImVec2 panelSize= ImGui::GetContentRegionAvail();
    if (ImGui::BeginTable("##Asset Browser", 2, ImGuiTableFlags_BordersInner | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY, panelSize)) {
        ImGui::TableSetupColumn("##FolderStructure", ImGuiTableColumnFlags_WidthStretch, 0.3f);
        ImGui::TableSetupColumn("##FilesRegion", ImGuiTableColumnFlags_WidthStretch, 0.7f);

        DrawProjectFolders();
        DrawAssets();
        ImGui::EndTable();
    }

	ImGui::End();
    IP_CORE_PROFILE_POP();
}

void AssetBrowser::ChangeBrowsingDirectory(std::filesystem::path directory) {
    m_CurrentBrowsingPath = directory;
}

void AssetBrowser::DrawProjectFolderRecursive(const std::filesystem::path& currentFolder) {

    for (const auto& directoryEntry : std::filesystem::directory_iterator(currentFolder)) {
        if (!directoryEntry.is_directory())
            continue;
        ImGuiTreeNodeFlags isSelectedFlag = (directoryEntry == m_CurrentBrowsingPath) ? ImGuiTreeNodeFlags_Selected : 0;
        ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | isSelectedFlag;
        bool nodeOpen = ImGui::TreeNodeEx(directoryEntry.path().string().c_str(), treeNodeFlags, "");
        //bool nodeOpen = ImGui::TreeNodeEx(directoryEntry.path().string().c_str(), treeNodeFlags, "%s %s", ICON_FA_FOLDER, directoryEntry.path().stem().string().c_str());

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IcePick::GetAssetTypeString(m_DragAssetType))) {
                HandleMoveAssetToDirectory(directoryEntry.path());
            }
            ImGui::EndDragDropTarget();
        }

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
            m_CurrentBrowsingPath = directoryEntry.path();
        }

        ImGui::SameLine();
        ImTextureID icon = (void*)m_Styles.GetIconTexture(Styles::ICON_FOLDER);
        ImGui::Image(icon, ImVec2(18.0f, 18.0f), ImVec2(0, 1), ImVec2(1, 0));

        ImGui::SameLine();
        ImGui::Text(directoryEntry.path().stem().string().c_str());

        if (nodeOpen) {
            DrawProjectFolderRecursive(directoryEntry.path());
            ImGui::TreePop();
        }
    }
}

void AssetBrowser::DrawProjectFolders() {
    ImGui::TableNextColumn();
    if (ImGui::BeginChild("ProjectFolderScrollRegion")) {
        DrawProjectFolderRecursive(m_ProjectDirectory);
        ImGui::EndChild();
    }
}

void AssetBrowser::DrawAssetActions() {
    if (ImGui::Button(ICON_FA_CHEVRON_LEFT)) {
        m_CurrentBrowsingPath = m_CurrentBrowsingPath.parent_path();
    }
    ImGui::SameLine();

    if (ImGui::Button("New")) {
        ImGui::OpenPopup("NEW_ASSET");
    }

    ImGui::SameLine();
    ImGui::TextUnformatted(ICON_FA_MAGNIFYING_GLASS);
    ImGui::SameLine();
    ImGui::TextUnformatted("Search");
    ImGui::SameLine();
    m_AssetNameFilter.Draw("##Search", 200.0f);
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_CIRCLE_XMARK)) {
        m_AssetNameFilter.Clear();
    }

    const auto& relativeBrowsingPath = std::filesystem::relative(std::filesystem::canonical(m_CurrentBrowsingPath), m_ProjectDirectory);
    std::filesystem::path breadCrumbPath;

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    for (const auto& pathSection : relativeBrowsingPath) {
        ImGui::PushID(pathSection.string().c_str());
        ImGui::SameLine();
        ImGui::Text("/");
        ImGui::SameLine();
        breadCrumbPath /= pathSection;
        if (ImGui::Button(pathSection.string().c_str())) {
            m_CurrentBrowsingPath = m_ProjectDirectory / breadCrumbPath;
        }
        ImGui::PopID();
    }
    ImGui::PopStyleColor();
    ImGui::Separator();
}

void AssetBrowser::DrawAssets() {
    ImGui::TableNextColumn();

    DrawAssetActions();
    
    if (ImGui::BeginChild("FileScrollRegion")) {

        // Configuration
        const float thumbnailSize = 110.0f;
        const float thumbnailPadding = 2.0f;
        const float cellPadding = 15.0f;
        const float cellWidth = thumbnailSize + cellPadding + thumbnailPadding;

        float drawRegionWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(drawRegionWidth / cellWidth);
        if (columnCount < 1)
            columnCount = 1;

        float textPadding = thumbnailPadding;

        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        ImVec2 drawRegionStart = ImGui::GetCursorScreenPos();
        float assertCardWidth = thumbnailSize + (2 *thumbnailPadding);
        float assetCardNameHeight = assertCardWidth / 2.0f;
        float assetCardRounding = 2.0f;
        ImVec2 assetCardSize{ assertCardWidth, assertCardWidth + assetCardNameHeight };

        unsigned int directoryEntryIndex = 0;
        for (const auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentBrowsingPath)) {
            ImGui::PushID(directoryEntryIndex);

            int rowIndex = (int)(directoryEntryIndex / columnCount);
            int columnIndex = directoryEntryIndex % columnCount;
            ImVec2 assetCardTopLeft{ drawRegionStart.x + (columnIndex * cellWidth), drawRegionStart.y + (rowIndex * (assetCardSize.y + cellPadding)) };
            ImVec2 assetCardBottomRight{ assetCardTopLeft.x + assetCardSize.x, assetCardTopLeft.y + assetCardSize.y };

            ImVec2 thumbnailTopLeft{ assetCardTopLeft.x + thumbnailPadding, assetCardTopLeft.y + thumbnailPadding };
            ImVec2 thumbnailBottomRight{ thumbnailTopLeft.x + thumbnailSize, thumbnailTopLeft.y + thumbnailSize };


            if (directoryEntry.is_directory()) {
                const std::string folderName = directoryEntry.path().stem().string();

                if (m_AssetNameFilter.PassFilter(folderName.c_str())) {
                    ImTextureID icon = (void*)m_Styles.GetIconTexture(Styles::ICON_FOLDER);
                    draw_list->AddImage(icon, thumbnailTopLeft, thumbnailBottomRight, ImVec2(0, 1), ImVec2(1, 0));

                    ImVec2 textSize = ImGui::CalcTextSize(folderName.c_str());

                    float folderTextWidth = std::min(textSize.x, assertCardWidth);
                    float textOffset = (assertCardWidth - folderTextWidth) / 2.0f;

                    ImVec2 fileNameTextPosition{ assetCardTopLeft.x + textPadding + textOffset, thumbnailBottomRight.y + textPadding };

                    ImGui::SetCursorScreenPos(fileNameTextPosition);
                    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + assertCardWidth - (2 * textPadding));
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));

                    ImGui::TextUnformatted(folderName.c_str());

                    ImGui::PopStyleColor();
                    ImGui::PopTextWrapPos();

                    ImGui::SetCursorScreenPos(assetCardTopLeft);
                    ImGui::InvisibleButton("DirectoryCard", assetCardSize, ImGuiButtonFlags_MouseButtonLeft);

                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IcePick::GetAssetTypeString(m_DragAssetType))) {
                            HandleMoveAssetToDirectory(directoryEntry.path());
                        }
                        ImGui::EndDragDropTarget();
                    }

                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                        ChangeBrowsingDirectory(directoryEntry.path());
                    }
                    directoryEntryIndex++;
                }

            }
            else if (directoryEntry.is_regular_file()) {
                const std::filesystem::path& fileExtension = directoryEntry.path().extension();
                std::string fileName = directoryEntry.path().stem().string();

                if (m_AssetNameFilter.PassFilter(fileName.c_str())) {
                    ImTextureID thumbnail = GetFileIcon(fileExtension);

                    if (fileExtension == ".iptex") {
                        std::filesystem::path fullAssetPath = std::filesystem::canonical(directoryEntry.path());
                        IcePick::UUID textureId = m_EngineAPI.LoadTextureFromAsset(fullAssetPath);
                        thumbnail = (void*)m_EngineAPI.GetTextureRenderId(textureId);
                    }

                    draw_list->AddRectFilled(assetCardTopLeft, assetCardBottomRight, IM_COL32(60, 60, 60, 255), assetCardRounding, ImDrawFlags_RoundCornersAll);
                    draw_list->AddRectFilled(thumbnailTopLeft, thumbnailBottomRight, IM_COL32(10, 10, 10, 255), assetCardRounding, ImDrawFlags_RoundCornersNone);
                    draw_list->AddImage(thumbnail, thumbnailTopLeft, thumbnailBottomRight, ImVec2(0, 1), ImVec2(1, 0));

                    ImVec2 fileNameTextPosition{ assetCardTopLeft.x + textPadding, thumbnailBottomRight.y + textPadding };

                    ImGui::SetCursorScreenPos(fileNameTextPosition);
                    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + assertCardWidth - (2 * textPadding));
                    ImGui::TextUnformatted(fileName.c_str());
                    ImGui::PopTextWrapPos();

                    ImGui::PushFont(m_Styles.GetSmallFont());
                    std::string fileType = IcePick::GetAssetTypeString(IcePick::GetAssetTypeFromExtension(fileExtension.string()));
                    ImVec2 fileTypeTextSize = ImGui::CalcTextSize(fileType.c_str());
                    ImVec2 fileTypeTextPosition{ assetCardBottomRight.x - textPadding - fileTypeTextSize.x, assetCardBottomRight.y - textPadding - fileTypeTextSize.y };
                    ImGui::SetCursorScreenPos(fileTypeTextPosition);

                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
                    ImGui::Text(fileType.c_str());
                    ImGui::PopStyleColor();
                    ImGui::PopFont();

                    ImGui::SetCursorScreenPos(assetCardTopLeft);
                    ImGui::InvisibleButton("AssetCard", assetCardSize, ImGuiButtonFlags_MouseButtonLeft);

                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                        m_DragFilePath = directoryEntry.path();
                        m_DragAssetType = IcePick::GetAssetTypeFromExtension(fileExtension.string());
                        ImGui::SetDragDropPayload(IcePick::GetAssetTypeString(m_DragAssetType), nullptr, 0, ImGuiCond_Once);

                        // Dragging asset preview
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                        ImGui::ImageButton("##Hello", thumbnail, ImVec2(40, 40), ImVec2(0, 1), ImVec2(1, 0), ImVec4(0, 0, 0, 1));
                        ImGui::PopStyleVar();

                        ImGui::SameLine();
                        ImGui::Text(fileName.c_str());
                        ImGui::EndDragDropSource();
                    }

                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                        HandleAssetDoubleClick(directoryEntry.path(), IcePick::GetAssetTypeFromExtension(fileExtension.string()));
                    }
                    directoryEntryIndex++;
                }
            }

            ImGui::PopID();
        }

        ImGui::EndChild();
    }
}

void AssetBrowser::HandleMoveAssetToDirectory(std::filesystem::path destinationDirectory) {
    std::filesystem::path fileName = m_DragFilePath.filename();
    std::filesystem::path destinationFilePath = destinationDirectory / fileName;
    std::filesystem::rename(m_DragFilePath, destinationFilePath);
}

void AssetBrowser::HandleAssetDoubleClick(std::filesystem::path assetPath, IcePick::AssetTypes assetType) {
    switch (assetType) {
    case IcePick::AssetTypes::STATIC_MESH:
    case IcePick::AssetTypes::SKELETAL_MESH:
        break;
    case IcePick::AssetTypes::MATERIAL_BASE:
        EditMaterialCallback(assetPath);
        break;
    case IcePick::AssetTypes::SCRIPT_ASSET:
        OpenScriptEditor(assetPath);
        break;
    default:
        break;
    }
}

void AssetBrowser::MaterialBasePopupOptions(const std::filesystem::path& filepath) {
    if (ImGui::Button("Edit material base")) {
        EditMaterialCallback(filepath);
    }

    if (ImGui::Button("Create material instance")) {
        ImGui::OpenPopup("Create Material Instance");
        ClearTextInputBuffer();
    }

    if (ImGui::BeginPopupModal("Create Material Instance")) {
        ImGui::InputText("File name (.ipmti)", TextInputBuffer, sizeof(TextInputBuffer));

        if (ImGui::Button("Save", ImVec2(120, 0))) {
            std::string fileName = std::string(TextInputBuffer) + ".ipmti";

            IcePick::UUID materialBaseId = m_EngineAPI.LoadMaterialBaseFromAsset(filepath);
            const IcePick::MaterialBase& materialBase = m_EngineAPI.GetMaterialBase(materialBaseId);
            std::filesystem::path materialInstancePath = m_CurrentBrowsingPath / fileName;
            IcePick::MaterialInstance newMaterialInstance = materialBase.CreateEmptyInstanceFromBase();

            m_EngineAPI.SerializeMaterialInstance(materialInstancePath, newMaterialInstance);
            ClearTextInputBuffer();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ClearTextInputBuffer();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

std::string AssetBrowser::GetDragFilePath() {
    return m_DragFilePath.string();
}

void* AssetBrowser::GetFileIcon(const std::filesystem::path& extension) {
    void* icon;
    if (extension == ".fbx") {
        icon = (void*)m_Styles.GetIconTexture(Styles::ICON_FBX_FILE);
    }
    else if (extension == ".glb") {
        icon = (void*)m_Styles.GetIconTexture(Styles::ICON_GLB_FILE);
    }
    else if (extension == ".obj") {
        icon = (void*)m_Styles.GetIconTexture(Styles::ICON_OBJ_FILE);
    }
    else if (extension == ".ipmtb" || extension == ".ipmti") {
        icon = (void*)m_Styles.GetIconTexture(Styles::ICON_MATERIAL_ASSET);
    }
    else if (extension == ".lua") {
        icon = (void*)m_Styles.GetIconTexture(Styles::ICON_SCRIPT_ASSET);
    }
    else {
        icon = (void*)m_Styles.GetIconTexture(Styles::ICON_GENERIC_FILE);
    }
    return icon;
}
