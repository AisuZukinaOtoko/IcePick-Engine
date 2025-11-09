#include "PropertiesPanel.h"
#include "PanelCommon.h"
#include "../Scene Systems/SceneRegistry.h"
#include <iostream>
#include <filesystem>

PropertiesPanel::PropertiesPanel(IcePick::EngineAPI engineAPI) :
    m_EngineAPI(engineAPI)
{
    m_ID = "Properties";
}

void PropertiesPanel::PanelSetup() {
    ImVec2 availableSpace = ImGui::GetContentRegionAvail();
    m_ColumnWidth = availableSpace.x / 3.0f;
}

void PropertiesPanel::Vec3Control(const char* label, glm::vec3& values, const float dragSpeed) {
    ImGui::PushID(label);

    // Styling
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, m_ColumnWidth);
    ImGui::Text(label);
    ImGui::NextColumn();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 3));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.85f, 0.3f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.2f, 0.0f, 1.0f));

    ImGui::Button("X");
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    ImGui::PushItemWidth(m_ColumnWidth / 2.0f);
    ImGui::DragFloat("##DragX", &values.x, dragSpeed, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.7f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.8f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.6f, 0.0f, 1.0f));

    ImGui::SameLine();
    ImGui::Button("Y");
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    ImGui::PushItemWidth(m_ColumnWidth / 2.0f);
    ImGui::DragFloat("##DragY", &values.y, dragSpeed, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.3f, 0.8f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.3f, 0.9f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.7f, 1.0f));

    ImGui::SameLine();
    ImGui::Button("Z");
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    ImGui::PushItemWidth(m_ColumnWidth / 2.0f);
    ImGui::DragFloat("##DragZ", &values.z, dragSpeed, 0.0f, 0.0f, "%.2f");

    ImGui::PopItemWidth();
    ImGui::Columns(1);
    ImGui::PopStyleVar(2);

    ImGui::PopID();
}

void PropertiesPanel::SelectedProperties(const Styles& styles) {
    ImGui::Begin(m_ID, nullptr, ImGuiWindowFlags_NoCollapse);
    PanelSetup();

    if (m_SelectedEntity == entt::null) {
        ImGui::End();
        return;
    }

    EntityProperties(styles);

    ImGui::End();
}

void PropertiesPanel::SetColumnWidth(float newWidth) {
    m_ColumnWidth = newWidth;
}

void PropertiesPanel::SetSelectedEntity(entt::entity entity) {
    m_SelectedEntity = entity;
}

void PropertiesPanel::SetDropAssetPath(std::string filePath) {
    m_DropAssetPath = filePath;
}

void PropertiesPanel::EntityProperties(const Styles& styles) {
    using namespace IcePick;
    ImVec2 windowSize = ImGui::GetWindowSize();

    if (HasComponent<TagComponent>(m_SelectedEntity)) {
        TagComponent& tag = GetComponent<TagComponent>(m_SelectedEntity);
        TextProperty("Name", tag.value.c_str());
    }

    if (HasComponent<TransformComponent>(m_SelectedEntity)) {
        ImGui::Spacing();
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            TransformComponent& transform = GetComponent<TransformComponent>(m_SelectedEntity);
            Vec3Control("Position", transform.Position, 0.05);
            Vec3Control("Rotation", transform.Rotation, 0.5);
            Vec3Control("Scale", transform.Scale, 0.03);
        }
    }

    if (HasComponent<MeshRendererComponent>(m_SelectedEntity)) {
        ImGui::Spacing();
        MeshRendererComponent& meshRenderer = GetComponent<MeshRendererComponent>(m_SelectedEntity);

        if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
            TextProperty("Mesh count", std::to_string(meshRenderer.MeshCount).c_str());
            CheckBox("Visible", &meshRenderer.MeshVisible);
            CheckBox("Cast shadows", &meshRenderer.CastShadows);
            CheckBox("Receive shadows", &meshRenderer.ReceiveShadows);

            ImGui::Text("Drop an asset here!");
            ImGui::NextColumn();
            ImGui::ImageButton("##MeshButton", (void*)styles.GetIconTexture(Styles::ICON_STATIC_MESH_ASSET), ImVec2(30, 30), ImVec2(0, 1), ImVec2(1, 0));
            if (ImGui::BeginDragDropTarget()) {
                ImGui::Text("Dropping something");
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET")) {
                    meshRenderer.MeshFilePath = m_DropAssetPath;
                    meshRenderer.MeshLoaded = false;
                }
                ImGui::EndDragDropTarget();
            }
        }

        if (!meshRenderer.MaterialSlots.empty() && ImGui::CollapsingHeader("Materials", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGuiTableFlags flags = ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_RowBg;
            flags = ImGuiTableFlags_Borders | ImGuiTableFlags_BordersInner;
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
                        if (ImGui::Button("Edit")) {
                            IP_LOG("Cannot edit material instance.", IP_ERROR_LOG);
                        }
                        ImGui::EndTable();
                    }

                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MATERIAL_INSTANCE_ASSET")) {
                            meshRenderer.MaterialSlots[i] = m_EngineAPI.LoadMaterialInstanceFromAsset(m_DropAssetPath);
                            IP_LOG(m_DropAssetPath.c_str(), IP_WARN_LOG);
                        }
                        ImGui::EndDragDropTarget();
                    }

                    MaterialInstanceParameters(materialBase, materialInstance);

                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
        }        
    }

    if (HasComponent<MaterialComponent>(m_SelectedEntity)) {
        MaterialComponent& material = GetComponent<MaterialComponent>(m_SelectedEntity);
        TextProperty("Material index", std::to_string(material.MaterialIndex).c_str());
    }

    if (HasComponent<RigidBodyComponent>(m_SelectedEntity)) {
        RigidBodyComponent& rigidBody = GetComponent<RigidBodyComponent>(m_SelectedEntity);
        Vec3Control("Velocity", rigidBody.Velocity, 0.01);
        FloatSlider("Mass", &rigidBody.Mass, 0.01f, 100.0f);
    }

    if (HasComponent<SphereColliderComponent>(m_SelectedEntity)) {
        SphereColliderComponent& sphereCollider = GetComponent<SphereColliderComponent>(m_SelectedEntity);
        Vec3Control("Offset", sphereCollider.PositionOffset, 0.01);
        FloatSlider("Radius", &sphereCollider.Radius, 0.0f, 100.0f);
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

void PropertiesPanel::FloatSlider(const char* label, float* value, float min, float max) {
    ImGui::PushID(label);
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, m_ColumnWidth);

    ImGui::Text(label);
    ImGui::NextColumn();
    ImGui::SliderFloat("##slider", value, min, max, "%.1f");

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
    ImGui::ColorEdit4("##Colour", &rgb.r, ImGuiColorEditFlags_PickerHueWheel);
    //ImGui::ColorPicker3("##Colour", &rgb.r, ImGuiColorEditFlags_PickerHueWheel);

    ImGui::Columns(1);
    ImGui::PopID();
}

void PropertiesPanel::MaterialInstanceParameters(IcePick::MaterialBase& materialBase, IcePick::MaterialInstance& materialInstance) {
    bool baseHasParameters = materialBase.MaterialTextures.size();

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

                        ImGui::Text("Material Texture");
                        if (ImGui::Button("Clear")) {
                            materialInstance.SetMaterialInstanceTextureId(baseDataId, IcePick::UUID::Unitialised());
                        }
                        ImGui::EndTable();
                    }

                    if (ImGui::BeginDragDropTarget()) {
                        if (ImGui::AcceptDragDropPayload("TEXTURE_ASSET")) {
                            IcePick::UUID droppedTextureId = m_EngineAPI.LoadTextureFromAsset(m_DropAssetPath);
                            materialInstance.SetMaterialInstanceTextureId(baseDataId, droppedTextureId);
                        }
                        ImGui::EndDragDropTarget();
                    }
                    ImGui::PopID();
                }

                    ImGui::EndTable();
            }
            ImGui::Unindent();
        }
    }
}
