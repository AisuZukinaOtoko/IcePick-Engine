#include "PropertiesPanel.h"
#include "PanelCommon.h"
#include "../Scene Systems/SceneRegistry.h"

PropertiesPanel::PropertiesPanel() {
    m_ID = "Properties";
}

void PropertiesPanel::PanelSetup() {
    ImVec2 availableSpace = ImGui::GetContentRegionAvail();
    ImGui::Text("Current available window region: %f, %f.", availableSpace.x, availableSpace.y);

    m_ColumnWidth = availableSpace.x / 3.0f;
    ImGui::Text("Current column width: %f", m_ColumnWidth);
}

void PropertiesPanel::Vec3Control(const char* label, glm::vec3& values) {
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
    ImGui::DragFloat("##DragX", &values.x, 0.01f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.7f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.8f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.6f, 0.0f, 1.0f));

    ImGui::SameLine();
    ImGui::Button("Y");
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    ImGui::PushItemWidth(m_ColumnWidth / 2.0f);
    ImGui::DragFloat("##DragY", &values.y, 0.01f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.3f, 0.8f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.3f, 0.9f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.7f, 1.0f));

    ImGui::SameLine();
    ImGui::Button("Z");
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    ImGui::PushItemWidth(m_ColumnWidth / 2.0f);
    ImGui::DragFloat("##DragZ", &values.z, 0.01f, 0.0f, 0.0f, "%.2f");

    ImGui::PopItemWidth();
    ImGui::Columns(1);
    ImGui::PopStyleVar(2);

    ImGui::PopID();
}

void PropertiesPanel::SelectedProperties() {
    ImGui::Begin(m_ID, nullptr, ImGuiWindowFlags_NoCollapse);
    PanelSetup();

    if (m_SelectedEntity == entt::null) {
        ImGui::End();
        return;
    }

    EntityProperties();

    ImGui::End();
}

void PropertiesPanel::SetColumnWidth(float newWidth) {
    m_ColumnWidth = newWidth;
}

void PropertiesPanel::SetEntitySelect(entt::entity entity) {
    m_SelectedEntity = entity;
}

void PropertiesPanel::EntityProperties() {
    using namespace IcePick;
    if (HasComponent<TagComponent>(m_SelectedEntity)) {
        TagComponent& tag = GetComponent<TagComponent>(m_SelectedEntity);
        TextProperty("Name", tag.value.c_str());
    }

    if (HasComponent<TransformComponent>(m_SelectedEntity)) {
        TransformComponent& transform = GetComponent<TransformComponent>(m_SelectedEntity);
        Vec3Control("Position", transform.Position);
        Vec3Control("Rotation", transform.Rotation);
        Vec3Control("Scale", transform.Scale);
    }

    if (HasComponent<MeshComponent>(m_SelectedEntity)) {
        MeshComponent& mesh = GetComponent<MeshComponent>(m_SelectedEntity);
        TextProperty("Vertex array ID", std::to_string(mesh.MeshVertexArrayRegistryIndex).c_str());
        TextProperty("Mesh type", std::to_string(mesh.MeshType).c_str());
        CheckBox("Visible", &mesh.MeshVisible);
        CheckBox("Cast shadows", &mesh.CastShadows);
        CheckBox("Receive shadows", &mesh.ReceiveShadows);
    }

    if (HasComponent<MaterialComponent>(m_SelectedEntity)) {
        MaterialComponent& material = GetComponent<MaterialComponent>(m_SelectedEntity);
        TextProperty("Material index", std::to_string(material.MaterialIndex).c_str());
    }

    if (HasComponent<RigidBodyComponent>(m_SelectedEntity)) {
        RigidBodyComponent& rigidBody = GetComponent<RigidBodyComponent>(m_SelectedEntity);
        Vec3Control("Velocity", rigidBody.Velocity);
        FloatSlider("Mass", &rigidBody.Mass, 0.01f, 100.0f);
    }

    if (HasComponent<SphereColliderComponent>(m_SelectedEntity)) {
        SphereColliderComponent& sphereCollider = GetComponent<SphereColliderComponent>(m_SelectedEntity);
        Vec3Control("Offset", sphereCollider.PositionOffset);
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

    float myColor[4] = { 0.2f, 0.4f, 0.8f, 1.0f };
    //ImGui::ColorEdit4("RGBA Color", myColor);
    //ImGui::ColorPicker3("RGBA Color 1", myColor);
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
