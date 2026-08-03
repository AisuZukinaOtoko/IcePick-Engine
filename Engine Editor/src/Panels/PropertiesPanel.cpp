#include "PropertiesPanel.h"
#include "PanelCommon.h"
#include "Scene Systems/SceneRegistry.h"
#include "Scene Systems/SceneCamera.h"
#include "Scene Systems/Components.h"
#include "File Systems/AssetTypes.h"
#include <IconsFontAwesome7.h>
#include "../Utils/Serialize.h"
#include <iostream>
#include <filesystem>

static char InputTextBuffer[30];

static void CameraControllerDropTargetProperty(const char* label, IcePick::SceneCamera& sceneCamera, entt::entity droppedSceneObject, float columnWidth) {
    ImGui::PushID(label);
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text(label);

    ImGui::NextColumn();

    entt::entity cameraControllerId = sceneCamera.GetCameraControllerId();
    std::string cameraControllerName = "None";
    if (IcePick::HasComponent<IcePick::TagComponent>(cameraControllerId)) {
        IcePick::TagComponent& tag = IcePick::GetComponent<IcePick::TagComponent>(cameraControllerId);
        cameraControllerName = tag.value;
    }

    std::string buttonText = std::string(ICON_FA_VIDEO) + " " + cameraControllerName;
    ImGui::Button(buttonText.c_str(), ImVec2(-FLT_MIN, 0.0f));

    if (ImGui::BeginDragDropTarget()) {
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CAMERA_CONTROLLER");
        if (payload) {
            SelectionContext droppedSelectionContext = *(SelectionContext*)payload->Data;
            sceneCamera.SetNewCameraController(static_cast<entt::entity>(droppedSelectionContext.SelectionId));
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::Columns(1);
    ImGui::PopID();
}

PropertiesPanel::PropertiesPanel(IcePick::EngineAPI engineAPI) :
    m_EngineAPI(engineAPI),
    m_PreviewRenderer(engineAPI)
{
    m_ID = "Properties";
    constexpr unsigned int previewRenderTargetSize = 600;
    m_PreviewRenderer.Init(previewRenderTargetSize, previewRenderTargetSize);
    m_PreviewRenderer.editorCamera.aspectRatio = 1.0f;
    m_SelectionContext.SelectionId = static_cast<uint64_t>(entt::null);
}

void PropertiesPanel::PanelSetup() {
    ImVec2 availableSpace = ImGui::GetContentRegionAvail();
    const float labelsToValuesColumnRatio = 45.0f / 100.0f;
    m_ColumnWidth = availableSpace.x * labelsToValuesColumnRatio;
    m_ValueColumnWidth = availableSpace.x - m_ColumnWidth;
}

void PropertiesPanel::Vec3Control(const char* label, glm::vec3& values, const float dragSpeed) {
    float vecComponentSpace = m_ValueColumnWidth / 3.0f;
    float labelToValueSpaceRatio = 26 / 100.0f;
    float vecComponentLabelSpace = vecComponentSpace * labelToValueSpaceRatio;
    float vecComponentValueSpace = vecComponentSpace * (1 - labelToValueSpaceRatio);

    ImGui::PushID(label);

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, m_ColumnWidth);
    ImGui::Text(label);
    ImGui::NextColumn();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 3));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.85f, 0.3f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.2f, 0.0f, 1.0f));

    ImGui::Button("X", ImVec2(vecComponentLabelSpace, 0.0f));
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(vecComponentValueSpace);
    ImGui::DragFloat("##DragX", &values.x, dragSpeed, 0.0f, 0.0f, "%.2f");

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.7f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.8f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.6f, 0.0f, 1.0f));

    ImGui::SameLine();
    ImGui::Button("Y", ImVec2(vecComponentLabelSpace, 0.0f));
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(vecComponentValueSpace);
    ImGui::DragFloat("##DragY", &values.y, dragSpeed, 0.0f, 0.0f, "%.2f");

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.3f, 0.8f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.3f, 0.9f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.7f, 1.0f));

    ImGui::SameLine();
    ImGui::Button("Z", ImVec2(vecComponentLabelSpace, 0.0f));
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(vecComponentValueSpace);
    ImGui::DragFloat("##DragZ", &values.z, dragSpeed, 0.0f, 0.0f, "%.2f");

    ImGui::Columns(1);
    ImGui::PopStyleVar(2);

    ImGui::PopID();
}

void PropertiesPanel::QuaternionEulerControls(const char* label, glm::quat& value, const float dragSpeed) {
    glm::vec3 eulerValues = glm::degrees(glm::eulerAngles(value));
    Vec3Control(label, eulerValues, dragSpeed);
    value = glm::quat(glm::radians(eulerValues));
}

void PropertiesPanel::SelectedProperties(const Styles& styles) {
    ImGui::Begin(m_ID, nullptr, ImGuiWindowFlags_NoCollapse);
    PanelSetup();
    entt::entity selectedEntity = static_cast<entt::entity>(m_SelectionContext.SelectionId);

    if (selectedEntity == entt::null) {
        ImGui::End();
        return;
    }

    m_MeshImportPopup.Render();
    EntityProperties(styles);

    ImGui::End();
}

void PropertiesPanel::SetColumnWidth(float newWidth) {
    m_ColumnWidth = newWidth;
}

void PropertiesPanel::SetSelectionContext(SelectionContext selectionContext) {
    m_SelectionContext = selectionContext;
}

void PropertiesPanel::SetDropEntity(entt::entity entity) {
    m_DroppedEntity = entity;
}

void PropertiesPanel::SetDropAssetPath(std::string filePath) {
    m_DropAssetPath = filePath;
}

void PropertiesPanel::EntityProperties(const Styles& styles) {
    using namespace IcePick;

    entt::entity selectedEntity = static_cast<entt::entity>(m_SelectionContext.SelectionId);

    if (HasComponent<TagComponent>(selectedEntity)) {
        TagComponent& tag = GetComponent<TagComponent>(selectedEntity);
        InputTextProperty("Name", tag.value);
    }

    if (HasComponent<TransformComponent>(selectedEntity)) {
        ImGui::Spacing();
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            TransformComponent& transform = GetComponent<TransformComponent>(selectedEntity);
            Vec3Control("Position", transform.Position, 0.05);
            QuaternionEulerControls("Rotation", transform.Rotation, 0.5);
            Vec3Control("Scale", transform.Scale, 0.03);
        }
    }

    if (HasComponent<SceneCamera>(selectedEntity)) {
        ImGui::Spacing();
        CameraDetails();
    }

    if (HasComponent<MeshRendererComponent>(selectedEntity)) {
        ImGui::Spacing();
        MeshRendererDetails(styles);
    }

    if (HasComponent<ScriptComponent>(selectedEntity)) {
        ImGui::Spacing();
        ScriptComponentDetails(styles);
    }

    if (HasComponent<RigidBodyComponent>(selectedEntity)) {
        ImGui::Spacing();
        RigidBodyComponentDetails(styles);
    }

    if (HasComponent<CameraControllerComponent>(selectedEntity)) {
        ImGui::Spacing();
        if (ImGui::CollapsingHeader("Camera Controller", ImGuiTreeNodeFlags_DefaultOpen)) {
            CameraControllerDetails();
        }
    }

    if (HasComponent<PointLightComponent>(selectedEntity)) {
        ImGui::Spacing();
        if (ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_DefaultOpen)) {
            PointLightComponent& pointLightComponent = GetComponent<PointLightComponent>(selectedEntity);
            ColourPicker("Colour", pointLightComponent.Colour);
            FloatSlider("Intensity", &pointLightComponent.Intensity, 0.0f, 10.0f);
        }
    }

    if (HasComponent<DirectionalLightComponent>(selectedEntity)) {
        ImGui::Spacing();
        if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen)) {
            DirectionalLightComponent& directionalLightComponent = GetComponent<DirectionalLightComponent>(selectedEntity);
            ColourPicker("Colour", directionalLightComponent.Colour);
            FloatSlider("Intensity", &directionalLightComponent.Intensity, 0.0f, 10.0f);
            FloatSlider("Azimuth", &directionalLightComponent.Azimuth, 0.0f, 360.0f);
            FloatSlider("Elevation", &directionalLightComponent.Elevation, -180.0f, 180.0f);
        }
    }
}

void PropertiesPanel::TextProperty(const char* label, const char* property) {
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, m_ColumnWidth);
    ImGui::Text(label);
    ImGui::NextColumn();
    ImGui::Text(property);
    ImGui::Columns(1);
}

void PropertiesPanel::InputTextProperty(const char* label, std::string& text) {
    ImGui::PushID(label);
    std::snprintf(InputTextBuffer, sizeof(InputTextBuffer), "%s", text.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, m_ColumnWidth);
    ImGui::Text(label);

    ImGui::NextColumn();
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(85, 85, 85, 120));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
    ImGui::SetNextItemWidth(-FLT_MIN); // Use all available horizontal space

    ImGui::InputText("##TextInput", InputTextBuffer, sizeof(InputTextBuffer));
    if (ImGui::IsItemDeactivated() && (strlen(InputTextBuffer) != 0)) {
        text = InputTextBuffer;
    }

    ImGui::PopStyleVar();
    ImGui::PopStyleColor();

    ImGui::Columns(1);
    ImGui::PopID();
}

void PropertiesPanel::FloatSlider(const char* label, float* value, float min, float max) {
    ImGui::PushID(label);
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, m_ColumnWidth);

    ImGui::Text(label);
    ImGui::NextColumn();
    ImGui::SetNextItemWidth(-FLT_MIN); // Use all available horizontal space
    ImGui::SliderFloat("##slider", value, min, max, "%.2f");

    ImGui::Columns(1);
    ImGui::PopID();
}

void PropertiesPanel::CheckBox(const char* label, bool* value) {
    ImGui::PushID(label);
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, m_ColumnWidth);

    ImGui::Text(label);
    ImGui::NextColumn();
    ImGui::Checkbox("##CheckBox", value);

    ImGui::Columns(1);
    ImGui::PopID();

}

void PropertiesPanel::ColourPicker(const char* label, glm::vec3& rgb) {
    ImGui::PushID(label);
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, m_ColumnWidth);

    ImGui::Text(label);
    ImGui::NextColumn();
    ImGui::ColorEdit3("##Colour", &rgb.r, ImGuiColorEditFlags_PickerHueWheel);

    ImGui::Columns(1);
    ImGui::PopID();
}

void PropertiesPanel::MaterialInstanceParameters(IcePick::MaterialBase& materialBase, IcePick::MaterialInstance& materialInstance) {
    bool baseHasParameters = materialBase.MaterialTextures.size() || materialBase.MaterialFloatParameters.size();

    if (baseHasParameters) {
        ImGui::TableNextRow(ImGuiTableRowFlags_None);
        ImGui::TableSetColumnIndex(0);

        if (ImGui::CollapsingHeader("Material Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent();
            if (ImGui::BeginTable("Texture parameters", 1)) {
                ImGui::TableNextRow(ImGuiTableRowFlags_None);
                ImGui::TableSetColumnIndex(0);

                for (int i = 0; i < materialBase.MaterialTextures.size(); i++) {
                    ImGui::PushID(i);
                    auto& baseTextureParam = materialBase.MaterialTextures[i];
                    IcePick::UUID baseDataId = baseTextureParam.Id;
                    IcePick::UUID parameterTextureId = materialInstance.GetMaterialInstanceTextureId(baseDataId);
                    unsigned int parameterTextureRenderId = m_EngineAPI.GetTextureRenderId(parameterTextureId);

                    if (ImGui::BeginTable("Material Texture", 2)) {
                        ImGui::TableNextRow(ImGuiTableRowFlags_None);
                        ImGui::TableNextColumn();
                        const int imageSize = 45;
                        ImGui::Image((void*)parameterTextureRenderId, ImVec2(imageSize, imageSize), ImVec2(0, 1), ImVec2(1, 0));

                        ImGui::TableNextColumn();

                        ImGui::Text(baseTextureParam.DisplayName.c_str());
                        if (ImGui::Button("Clear")) {
                            materialInstance.SetMaterialInstanceTextureId(baseDataId, IcePick::UUID::Unitialised());
                            m_EngineAPI.UpdateMaterialInstance(materialInstance.Id, materialInstance); // Calling update will invalidate the cache. This is desired.
                        }
                        ImGui::EndTable();
                    }

                    if (ImGui::BeginDragDropTarget()) {
                        if (ImGui::AcceptDragDropPayload(IcePick::GetAssetTypeString(IcePick::AssetTypes::TEXTURE))) {
                            IcePick::UUID droppedTextureId = m_EngineAPI.LoadTextureFromAsset(m_DropAssetPath);
                            materialInstance.SetMaterialInstanceTextureId(baseDataId, droppedTextureId);
                            m_EngineAPI.UpdateMaterialInstance(materialInstance.Id, materialInstance); // Calling update will invalidate the cache. This is desired.
                        }
                        ImGui::EndDragDropTarget();
                    }
                    ImGui::PopID();
                }

                    ImGui::EndTable();
            }

            if (ImGui::BeginTable("Float parameters", 1)) {
                ImGui::TableNextRow(ImGuiTableRowFlags_None);
                ImGui::TableSetColumnIndex(0);

                for (int i = 0; i < materialBase.MaterialFloatParameters.size(); i++) {
                    ImGui::PushID(i);
                    auto& baseFloatParameter = materialBase.MaterialFloatParameters[i];

                    if (ImGui::BeginTable("Instance Float Parameter", 2)) {
                        ImGui::TableNextRow(ImGuiTableRowFlags_None);
                        ImGui::TableNextColumn();

                        ImGui::Text(baseFloatParameter.DisplayName.c_str());
                        ImGui::TableNextColumn();

                        ImGui::SetNextItemWidth(-FLT_MIN); // Use all available horizontal space.
                        for (int j = 0; j < materialInstance.InstanceFloatData.size(); j++) {
                            auto& instanceFloatData = materialInstance.InstanceFloatData[j];

                            if (instanceFloatData.MaterialBaseDataId != baseFloatParameter.Id)
                                continue;

                            if (ImGui::DragFloat("##Material Instance Float", &instanceFloatData.Data, 0.005)) {
                                m_EngineAPI.UpdateMaterialInstance(materialInstance.Id, materialInstance); // Calling update will invalidate the cache. This is desired.
                            }
                        }                        
                        ImGui::EndTable();
                    }
                    ImGui::PopID();
                }

                ImGui::EndTable();
            }

            ImGui::Unindent();
        }
    }
}

void PropertiesPanel::EntityDropTargetProperty(const char* label, entt::entity& entityProperty) {
    ImGui::PushID(label);
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, m_ColumnWidth);
    ImGui::Text(label);

    ImGui::NextColumn();

    std::string entityName = "None";
    if (IcePick::HasComponent<IcePick::TagComponent>(entityProperty)) {
        IcePick::TagComponent& tag = IcePick::GetComponent<IcePick::TagComponent>(entityProperty);
        entityName = tag.value;
    }

    std::string buttonText = std::string(ICON_FA_CUBE) + " " + entityName;
    ImGui::Button(buttonText.c_str(), ImVec2(-FLT_MIN, 0.0f));

    if (ImGui::BeginDragDropTarget()) {
        const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_ENTITY");
        if (payload) {
            SelectionContext droppedSelectionContext = *(SelectionContext*)payload->Data;
            entityProperty = static_cast<entt::entity>(droppedSelectionContext.SelectionId);
        }
        ImGui::EndDragDropTarget();
    }

    ImGui::Columns(1);
    ImGui::PopID();
}

void PropertiesPanel::CameraDetails() {
    entt::entity selectedEntity = static_cast<entt::entity>(m_SelectionContext.SelectionId);
    IcePick::SceneCamera& sceneCamera = IcePick::GetComponent<IcePick::SceneCamera>(selectedEntity);
    CameraControllerDropTargetProperty("Default camera controller", sceneCamera, m_DroppedEntity, m_ColumnWidth);
}

void PropertiesPanel::MeshRendererDetails(const Styles& styles) {
    entt::entity selectedEntity = static_cast<entt::entity>(m_SelectionContext.SelectionId);
    IcePick::MeshRendererComponent& meshRenderer = IcePick::GetComponent<IcePick::MeshRendererComponent>(selectedEntity);

    if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
        switch (meshRenderer.MeshType) {
        case IcePick::ImportSettings::MeshType::STATIC_MESH:
            TextProperty("Mesh type", "Static Mesh");
            break;
        case IcePick::ImportSettings::MeshType::SKELETAL_MESH:
            TextProperty("Mesh type", "Skeletal Mesh");
            break;
        }

        TextProperty("Mesh count", std::to_string(meshRenderer.MeshCount).c_str());
        CheckBox("Visible", &meshRenderer.MeshVisible);
        CheckBox("Cast shadows", &meshRenderer.CastShadows);
        CheckBox("Receive shadows", &meshRenderer.ReceiveShadows);

        ImGui::Text("Drop an asset here!");
        ImGui::NextColumn();
        ImGui::ImageButton("##MeshButton", (void*)styles.GetIconTexture(Styles::ICON_STATIC_MESH_ASSET), ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0));
        if (ImGui::BeginDragDropTarget()) {
            ImGui::Text("Dropping something");
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IcePick::GetAssetTypeString(IcePick::AssetTypes::STATIC_MESH))) {
                //meshRenderer.MeshFilePath = m_DropAssetPath;
                //meshRenderer.MeshLoaded = false;
                m_MeshImportPopup.OpenPopup();
            }
            ImGui::EndDragDropTarget();
        }
        if (m_MeshImportPopup.ImportSubmitted()) {
            meshRenderer = m_EngineAPI.LoadMesh(m_DropAssetPath, m_MeshImportPopup.GetImportSettings());
            m_MeshImportPopup.HandleSubmit();
        }
    }

    if (!meshRenderer.MaterialSlots.empty() && ImGui::CollapsingHeader("Materials", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGuiTableFlags flags = ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_RowBg;
        flags |= ImGuiTableFlags_Borders | ImGuiTableFlags_BordersInner;
        if (ImGui::BeginTable("table_nested1", 1, flags)) {

            for (unsigned int i = 0; i < meshRenderer.MaterialSlots.size(); i++) {
                IcePick::UUID materialInstanceId = meshRenderer.MaterialSlots[i];
                IcePick::MaterialInstance& materialInstance = m_EngineAPI.GetMaterialInstance(materialInstanceId);
                IcePick::MaterialBase& materialBase = m_EngineAPI.GetMaterialBase(materialInstance.MaterialBaseId);

                ImGui::PushID(i);
                ImGui::TableNextRow(ImGuiTableRowFlags_None);
                ImGui::TableSetColumnIndex(0);

                if (ImGui::BeginTable("Material", 2)) {
                    ImGui::TableNextRow(ImGuiTableRowFlags_None);
                    ImGui::TableNextColumn();
                    const int imageSize = 45;
                    ImGui::ImageButton("##MaterialButton", (void*)styles.GetIconTexture(Styles::ICON_MATERIAL_ASSET), ImVec2(imageSize, imageSize), ImVec2(0, 1), ImVec2(1, 0));

                    ImGui::TableNextColumn();

                    ImGui::Text("Material: %d", i);

                    ImGui::EndTable();
                }

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IcePick::GetAssetTypeString(IcePick::AssetTypes::MATERIAL_INSTANCE))) {
                        meshRenderer.MaterialSlots[i] = m_EngineAPI.LoadMaterialInstanceFromAsset(m_DropAssetPath);
                    }
                    ImGui::EndDragDropTarget();
                }

                MaterialInstanceParameters(materialBase, materialInstance);

                ImGui::PopID();

                /*if (i < meshRenderer.MaterialSlots.size() - 1) {
                    ImGui::Separator();
                }*/
            }
            ImGui::EndTable();
        }
    }
}

void PropertiesPanel::ScriptComponentDetails(const Styles& styles) {
    entt::entity selectedEntity = static_cast<entt::entity>(m_SelectionContext.SelectionId);
    if (ImGui::CollapsingHeader("Scripts", ImGuiTreeNodeFlags_DefaultOpen)) {
        IcePick::ScriptComponent& scriptComponent = IcePick::GetComponent<IcePick::ScriptComponent>(selectedEntity);

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, m_ColumnWidth);
        ImGui::Text("Script");
        ImGui::NextColumn();

        if (ImGui::Button(ICON_FA_TRASH)) {
            scriptComponent.ScriptId = IcePick::UUID::Unitialised();
            scriptComponent.Active = false;
        }

        ImGui::SameLine();
        std::filesystem::path scriptPath = m_EngineAPI.GetScriptPathById(scriptComponent.ScriptId);
        std::string buttonText;
        if (scriptPath.empty())
            buttonText = std::string(ICON_FA_EXCLAMATION) + " None";
        else
            buttonText = std::string(ICON_FA_FILE) + " " + scriptPath.stem().string();
        ImGui::Button(buttonText.c_str(), ImVec2(-FLT_MIN, 0.0f));

        if (ImGui::BeginDragDropTarget()) {
            if (ImGui::AcceptDragDropPayload(IcePick::GetAssetTypeString(IcePick::AssetTypes::SCRIPT_ASSET))) {
                scriptComponent = m_EngineAPI.LoadScript(m_DropAssetPath, selectedEntity);
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::Columns(1);

        CheckBox("Script active", &scriptComponent.Active);
        
        if (ImGui::Button(ICON_FA_PEN_TO_SQUARE "Edit Script")) {
            OpenScriptEditor(scriptPath);
        }
    }
}