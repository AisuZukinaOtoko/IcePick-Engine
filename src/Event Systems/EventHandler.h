#pragma once
#include "Event.h"
#include "GLFW/glfw3.h"
#include "../Vendor/glm/glm.hpp"

constexpr auto IP_PRESS = GLFW_PRESS;
constexpr auto IP_RELEASE = GLFW_RELEASE;
constexpr auto IP_CONNECT = GLFW_CONNECTED;
constexpr auto IP_DISCONNECT = GLFW_DISCONNECTED;

namespace IcePick {
	using ControllerState = GLFWgamepadstate;

	class EventHandler {
	public:
		bool Init();
		glm::vec2 GetCursorPos();
		void OnEvent(Event& event);
		void NewFrame();
		float GetControllerAxis(unsigned int controllerId, unsigned int axisId);
	private:
		static constexpr unsigned int m_MaxControllerCount = 4;
		ControllerState m_ControllerStates[m_MaxControllerCount];
		bool InitForController();
	};
	inline EventHandler IP_EventHandler;
}