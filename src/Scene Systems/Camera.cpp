#include "Camera.h"
#include "../Vendor/glm/gtc/matrix_transform.hpp"

IcePick::Camera::Camera(glm::vec3 pos, glm::vec3 rotation, Projection proj) : m_Position(pos), m_Rotation(rotation) {

}

glm::mat4 IcePick::Camera::GetViewMatrix() {
	glm::vec3 front;
	front.x = cos(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x));
	front.y = sin(glm::radians(m_Rotation.x));
	front.z = sin(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x));
	front = glm::normalize(front);

	glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
	glm::vec3 up = glm::normalize(glm::cross(right, front));


	glm::vec3 center = m_Position + front;
	glm::mat4 view = glm::lookAt(m_Position, center, up);

	return view;
	//return glm::lookAt(m_Position, m_Rotation, glm::vec3(0.0f, 1.0f, 0.0f));
	//return glm::mat4();
}

glm::mat4 IcePick::Camera::GetViewProjectionMatrix() {
	return glm::mat4();
}

glm::mat4 IcePick::Camera::GetProjection() {
	return glm::mat4();
}

void IcePick::Camera::SetCameraClip(float nearClip, float farClip) {
	m_NearClip = nearClip;
	m_FarClip = farClip;
}
