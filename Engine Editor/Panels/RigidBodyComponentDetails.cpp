#include "PropertiesPanel.h"

static const char* RigidBodyMotionTypeToString(IcePick::RigidBodyComponent::MotionTypes motionType) {
    switch (motionType) {
    case IcePick::RigidBodyComponent::MotionTypes::STATIC:
        return "Static";
    case IcePick::RigidBodyComponent::MotionTypes::DYNAMIC:
        return "Dynamic";
    case IcePick::RigidBodyComponent::MotionTypes::KINEMATIC:
        return "Kinematic";
    default:
        return "Error";
    }
}

static const char* RigidBodyColliderShapeTypeToString(IcePick::ColliderShape::ColliderShapeType colliderShapeType) {
    switch (colliderShapeType) {
    case IcePick::ColliderShape::ColliderShapeType::BOX_SHAPE:
        return "Box Shape";
    case IcePick::ColliderShape::ColliderShapeType::SPHERE_SHAPE:
        return "Sphere Shape";
    case IcePick::ColliderShape::ColliderShapeType::CAPSULE_SHAPE:
        return "Capsule Shape";
    default:
        return "Error";
    }
}

void PropertiesPanel::RigidBodyComponentDetails(const Styles& styles) {
    if (ImGui::CollapsingHeader("Rigid Body", ImGuiTreeNodeFlags_DefaultOpen)) {
        IcePick::RigidBodyComponent& rigidBodyComponent = IcePick::GetComponent<IcePick::RigidBodyComponent>(m_SelectedEntity);

        TextProperty("Collider Count", std::to_string(rigidBodyComponent.ColliderShapeCount).c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, m_ColumnWidth);
        ImGui::Text("Motion Type");
        ImGui::NextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN); // Use all available horizontal space
        if (ImGui::BeginCombo("##MotionType", RigidBodyMotionTypeToString(rigidBodyComponent.MotionType))) {
            for (int i = 0; i < (int)IcePick::RigidBodyComponent::MotionTypes::MOTION_TYPE_COUNT; ++i) {
                IcePick::RigidBodyComponent::MotionTypes value = static_cast<IcePick::RigidBodyComponent::MotionTypes>(i);
                bool selected = (rigidBodyComponent.MotionType == value);

                if (ImGui::Selectable(RigidBodyMotionTypeToString(value), selected))
                    rigidBodyComponent.MotionType = value;

                if (selected)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }
        ImGui::Columns(1);

        ImGui::SeparatorText("Colliders");

        ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders;

        for (unsigned int i = 0; i < rigidBodyComponent.ColliderShapeCount; i++) {
            IcePick::ColliderShape& collider = rigidBodyComponent.ColliderShapes[i];
            ImGui::PushID(i);

            if (ImGui::BeginTable("Instance Float Parameter", 1), tableFlags) {
                ImGui::TableNextRow(ImGuiTableRowFlags_None);
                ImGui::TableNextColumn();

                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, m_ColumnWidth);
                ImGui::Text("Collider Shape");
                ImGui::NextColumn();
                ImGui::SetNextItemWidth(-FLT_MIN); // Use all available horizontal space
                if (ImGui::BeginCombo("##ColliderShapeType", RigidBodyColliderShapeTypeToString(collider.ShapeType))) {
                    for (int i = 0; i < (int)IcePick::ColliderShape::ColliderShapeType::COLLIDER_SHAPE_COUNT; ++i) {
                        IcePick::ColliderShape::ColliderShapeType value = static_cast<IcePick::ColliderShape::ColliderShapeType>(i);
                        bool selected = (collider.ShapeType == value);

                        if (ImGui::Selectable(RigidBodyColliderShapeTypeToString(value), selected))
                            collider.ShapeType = value;

                        if (selected)
                            ImGui::SetItemDefaultFocus();
                    }

                    ImGui::EndCombo();
                }
                ImGui::Columns(1);

                ImGui::Spacing();
                Vec3Control("Collider Offset", collider.ColliderOffset, 0.1f);

                switch (collider.ShapeType) {
                case IcePick::ColliderShape::ColliderShapeType::BOX_SHAPE:
                    ImGui::Spacing();
                    Vec3Control("Box Scale", collider.ColliderScale, 0.1f);
                    break;
                case IcePick::ColliderShape::ColliderShapeType::SPHERE_SHAPE:
                    ImGui::Spacing();
                    FloatSlider("Sphere Radius", &collider.Radius, 0.1f, 15.0f);
                    break;
                case IcePick::ColliderShape::ColliderShapeType::CAPSULE_SHAPE:
                    ImGui::Spacing();
                    FloatSlider("Capsule Height", &collider.ColliderScale.y, 0.1f, 15.0f);
                    ImGui::Spacing();
                    FloatSlider("Capsule Radius", &collider.Radius, 0.1f, 15.0f);
                    break;
                }                
                
                ImGui::EndTable();
            }

            ImGui::Spacing();
            ImGui::PopID();
        }

        bool canAddCollider = rigidBodyComponent.ColliderShapeCount < rigidBodyComponent.MaxColliderShapeCount;
        ImGui::BeginDisabled(!canAddCollider);
        if (ImGui::Button("Add Collider")) {
            rigidBodyComponent.ColliderShapeCount++;
        }
        ImGui::EndDisabled();

    }
}
