#pragma once
#include "glm/glm.hpp"
#include "../Utilities/Clock.h"

class EditorCamera {
public:
	EditorCamera();
	~EditorCamera();
	glm::vec3 cameraPosition;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;

	float yaw = 0.0f;
	float pitch = 0.0f;
	float aspectRatio = 1.0f;
	float cameraFOV = 1.0f;

	float cameraNearClip = 0.1f;
	float cameraFarClip = 1000.0f;

	glm::mat4 getViewProjectionMatrix();
	void OnUpdate(DeltaTime dt);
	void OnKeyPress(int action, int code);
private:
	bool m_LeftStrafe = false;
	bool m_RightStrafe = false;
	bool m_ForwardStrafe = false;
	bool m_BackStrafe = false;
	bool m_UpFloat = false;
	bool m_DownFloat = false;
};