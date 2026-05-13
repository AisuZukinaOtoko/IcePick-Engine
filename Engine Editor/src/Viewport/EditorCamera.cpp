#include "EditorCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Utilities/DebugStatistics.h"
#include "Event Systems/Input.h"
#include "Event Systems/EventHandler.h"

EditorCamera::EditorCamera() {
    cameraPosition = glm::vec3(1.5f, 2.25f, 1.5f);
    cameraFront = glm::vec3(0.0f) - cameraPosition;
    cameraUp = {0.0f, 1.0f, 0.0f};

    yaw = -135.0f;
    pitch = 35.0f;
    aspectRatio = 1920.0f / 1080.0f;
    cameraFOV = glm::radians(45.0f);
}

glm::mat4 EditorCamera::GetViewProjectionMatrix() {
    return GetProjectionMatrix() * GetViewMatrix();
}

glm::mat4 EditorCamera::GetProjectionMatrix() {
    return glm::perspective(cameraFOV, aspectRatio, cameraNearClip, cameraFarClip);
}

glm::mat4 EditorCamera::GetViewMatrix() {
    return glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
}

void EditorCamera::OnUpdate(DeltaTime dt) {
    // calculate camera's forward vector
    if (pitch > 89.0f)  
        pitch = 89.0f;
    if (pitch < -89.0f) 
        pitch = -89.0f;
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = -sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);

    // move camera
    if (m_ForwardStrafe) {
        cameraPosition += cameraFront * moveSensitivity * dt.GetDelta();
    }
    if (m_BackStrafe) {
        cameraPosition -= cameraFront * moveSensitivity * dt.GetDelta();
    }

    if (m_LeftStrafe) {
        cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * moveSensitivity * dt.GetDelta();
    }

    if (m_RightStrafe) {
        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * moveSensitivity * dt.GetDelta();
    }

    if (m_UpFloat) {
        cameraPosition += cameraUp * moveSensitivity * dt.GetDelta();
    }
    if (m_DownFloat) {
        cameraPosition -= cameraUp * moveSensitivity * dt.GetDelta();
    }
}

void EditorCamera::OnKeyPress(int action, int code) {
    switch (code) {
    case IcePick::IP_KEY_E:
        m_UpFloat = (action != IP_RELEASE);
        break;
    case IcePick::IP_KEY_Q:
        m_DownFloat = (action != IP_RELEASE);
        break;
    case IcePick::IP_KEY_W:
        m_ForwardStrafe = (action != IP_RELEASE);
        break;
    case IcePick::IP_KEY_A:
        m_LeftStrafe = (action != IP_RELEASE);
        break;
    case IcePick::IP_KEY_S:
        m_BackStrafe = (action != IP_RELEASE);
        break;
    case IcePick::IP_KEY_D:
        m_RightStrafe = (action != IP_RELEASE);
        break;
    case IcePick::IP_MOUSE_RIGHT:
        m_UpFloat = m_DownFloat = m_ForwardStrafe = m_LeftStrafe = m_BackStrafe = m_RightStrafe = (action != IP_RELEASE); // disable movement if right mouse button is released
        break;
    default:
        break;
    }
}

EditorCamera::~EditorCamera() {

}