#include "AssetBrowser.h"
#include <filesystem>
#include "../Utils/Serialize.h"
#include "Utilities/DebugStatistics.h"

static const unsigned int bufferSize = 255;
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

    if (ImGui::Button("Back")) {
        m_CurrentBrowsingPath = m_CurrentBrowsingPath.parent_path();
    }
    ImGui::SameLine();

    if (ImGui::Button("New")) {
        ImGui::OpenPopup("NEW_ASSET");
    }


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

    // Configuration
    const float iconSize = 90.0f;
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
        std::string assetType = "ASSET";
        std::filesystem::path extension = file.path().extension();
        ImTextureID icon = GetFileIcon(extension);

        if (file.is_regular_file()) {

            if (extension == ".iptex") {
                assetType = "TEXTURE_ASSET";
                std::filesystem::path fullAssetPath = std::filesystem::canonical(file.path());
                IcePick::UUID textureId = m_EngineAPI.LoadTextureFromAsset(fullAssetPath);
                icon = (void*)m_EngineAPI.GetTextureRenderId(textureId);
            }
            else if (extension == ".ipmtb") {
                std::filesystem::path fullAssetPath = std::filesystem::canonical(file.path());
                m_EngineAPI.LoadMaterialBaseFromAsset(file.path());
                assetType = "MATERIAL_BASE_ASSET";                
            }
            else if (extension == ".ipmti") {
                assetType = "MATERIAL_INSTANCE_ASSET";
            }
            else if (extension == ".lua") {
                assetType = "SCRIPT_ASSET";
            }
        }
        else if (file.is_directory()) {
            icon = (void*)m_Styles.GetIconTexture(Styles::ICON_FOLDER);
        }
        

        ImGui::PushID(tempIterator);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));
        ImGui::ImageButton("##Hello", icon, ImVec2(iconSize, iconSize), ImVec2(0, 1), ImVec2(1, 0), ImVec4(0, 0, 0, 1));
        ImGui::PopStyleVar(3);

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            // Open asset or navigate into folder
            if (file.is_directory()) {
                m_CurrentBrowsingPath = file.path();
            }
            else if (file.is_regular_file()) {
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && extension == ".ipmtb") {
                    EditMaterialCallback(file.path());
                }
                else if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && extension == ".lua") {
                    OpenScriptEditor(file.path());
                }
            }            
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            if (file.is_regular_file()) {
                ImGui::OpenPopup("GENERIC_FILE_POPUP");
            }
        }

        if (ImGui::BeginPopup("GENERIC_FILE_POPUP")) {
            if (extension == ".ipmtb") {
                MaterialBasePopupOptions(file.path());
            }

            if (ImGui::Button("Delete")) {
                std::error_code error;
                std::filesystem::remove(file.path(), error);
                if (error) {
                    IP_LOG(error.message(), IP_ERROR_LOG);
                }
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            std::filesystem::path assetPath = file.path();

            m_DragFilePath = assetPath.string();
            ImGui::SetDragDropPayload(assetType.c_str(), nullptr, 0, ImGuiCond_Once);

            // Optionally show preview while dragging
            ImGui::ImageButton("##Hello", icon, ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0), ImVec4(0, 0, 0, 1));
            ImGui::SameLine();
            ImGui::Text(file.path().filename().string().c_str());
            ImGui::EndDragDropSource();
        }

        ImGui::TextWrapped(file.path().filename().string().c_str());

        ImGui::NextColumn();
        ImGui::PopID();
    }    

    ImGui::Columns(1);
	ImGui::End();
    IP_CORE_PROFILE_POP();
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
    return m_DragFilePath;
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
