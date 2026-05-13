#include "PropertiesPanel.h"
#include "Scene Systems/Components.h"

static const char* CameraControllerModeToString(IcePick::CameraControllerComponent::ControllerMode mode) {
    switch (mode) {
    case IcePick::CameraControllerComponent::ControllerMode::NONE:
        return "None";
    case IcePick::CameraControllerComponent::ControllerMode::FOLLOW:
        return "Follow (Not implemented)";
    case IcePick::CameraControllerComponent::ControllerMode::THIRD_PERSON:
        return "Third Person Camera";
    default:
        return "Error";
    }
}

static const char* CameraControllerInterpolationToString(IcePick::CameraControllerComponent::Interpolation interpolation) {
    switch (interpolation) {
    case IcePick::CameraControllerComponent::Interpolation::NONE:
        return "None";
    case IcePick::CameraControllerComponent::Interpolation::LINEAR:
        return "Linear";
    default:
        return "Error";
    }
}

void PropertiesPanel::CameraControllerDetails() {
    using namespace IcePick;
    CameraControllerComponent& cameraController = IcePick::GetComponent<CameraControllerComponent>(m_SelectedEntity);

    Vec3Control("Position", cameraController.Position, 0.2f);

    ImGui::Spacing();
    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, m_ColumnWidth);
    ImGui::Text("Mode");
    ImGui::NextColumn();
    ImGui::SetNextItemWidth(-FLT_MIN); // Use all available horizontal space
    if (ImGui::BeginCombo("##ControllerMode", CameraControllerModeToString(cameraController.Mode))) {
        for (int i = 0; i < (int)CameraControllerComponent::ControllerMode::COUNT; ++i) {
            CameraControllerComponent::ControllerMode value = static_cast<CameraControllerComponent::ControllerMode>(i);
            bool selected = (cameraController.Mode == value);

            if (ImGui::Selectable(CameraControllerModeToString(value), selected))
                cameraController.Mode = value;

            if (selected)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }
    ImGui::Columns(1);

    //ImGui::Spacing();
    //EntityDropTargetProperty("Follow Target", cameraController.FollowTarget);

    ImGui::Spacing();
    EntityDropTargetProperty("Look At Target", cameraController.LookAtTarget);

    //ImGui::SeparatorText("Interpolation");

    //ImGui::Columns(2);
    //ImGui::SetColumnWidth(0, m_ColumnWidth);
    //ImGui::Text("Interpolation Style");
    //ImGui::NextColumn();
    //ImGui::SetNextItemWidth(-FLT_MIN); // Use all available horizontal space
    //if (ImGui::BeginCombo("##InterpolationMode", CameraControllerInterpolationToString(cameraController.EnterInterpolation))) {
    //    for (int i = 0; i < (int)CameraControllerComponent::Interpolation::COUNT; ++i) {
    //        CameraControllerComponent::Interpolation value = static_cast<CameraControllerComponent::Interpolation>(i);
    //        bool selected = (cameraController.EnterInterpolation == value);

    //        if (ImGui::Selectable(CameraControllerInterpolationToString(value), selected))
    //            cameraController.EnterInterpolation = value;

    //        if (selected)
    //            ImGui::SetItemDefaultFocus();
    //    }

    //    ImGui::EndCombo();
    //}
    //ImGui::Columns(1);

    //ImGui::Spacing();
    //FloatSlider("Duration", &cameraController.InterpolationDuration, 0.0f, 10.0f);


    //ImGui::SeparatorText("View Settings");
    //FloatSlider("FOV", &cameraController.FOV, 1.0f, 179.0f);
}