#include "EditorCamera.h"
#include "../IcePickCoreMinimal.h"
#include "../Vendor/glm/gtc/matrix_transform.hpp"

EditorCamera::EditorCamera() {
    cameraPosition = glm::vec3(0.0f);
    cameraFront = glm::vec3(0.0f);
    cameraUp = {0.0f, 1.0f, 0.0f};

    yaw = glm::radians(45.0f);
    pitch = glm::radians(30.0f);
    aspectRatio = 1920.0f / 1080.0f;
    cameraFOV = glm::radians(45.0f);
}

glm::mat4 EditorCamera::getViewProjectionMatrix() {
    glm::mat4 projection = glm::perspective(cameraFOV, aspectRatio, cameraNearClip, cameraFarClip);
    glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
    return projection * view;
}

void EditorCamera::OnUpdate(DeltaTime dt) {
    // calculate camera's forward vector
    if (pitch > 89.0f)  
        pitch = 89.0f;
    if (pitch < -89.0f) 
        pitch = -89.0f;
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);

    // move camera
    if (m_ForwardStrafe) {
        cameraPosition += cameraFront;
    }
    if (m_BackStrafe) {
        cameraPosition -= cameraFront;
    }

    if (m_LeftStrafe) {
        cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp));
    }

    if (m_RightStrafe) {
        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp));
    }

    if (m_UpFloat) {
        cameraPosition -= cameraUp;
    }
    if (m_DownFloat) {
        cameraPosition += cameraUp;
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