#pragma once
#include "../Vendor/glm/glm.hpp"

namespace IcePick {
	class Camera {
		enum Projection {PERSPECTIVE, ORTHOGRAPHIC};
	public:
		Camera(glm::vec3 pos, glm::vec3 rotation, Projection proj);
		glm::mat4 GetViewMatrix();
		glm::mat4 GetViewProjectionMatrix();
		glm::mat4 GetProjection();

		void SetCameraClip(float nearClip, float farClip);

	private:
		glm::vec3 m_Position;
		glm::vec3 m_Rotation;
		glm::mat4 m_Projection;
		float m_FOV = 45.0f;
		float m_NearClip = 0.1f;
		float m_FarClip = 100.0f;
	};
}