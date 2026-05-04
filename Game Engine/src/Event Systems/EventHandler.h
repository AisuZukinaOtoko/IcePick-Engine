#pragma once
#include "Event.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>

constexpr auto IP_PRESS = GLFW_PRESS;
constexpr auto IP_RELEASE = GLFW_RELEASE;
constexpr auto IP_CONNECT = GLFW_CONNECTED;
constexpr auto IP_DISCONNECT = GLFW_DISCONNECTED;

namespace IcePick {
	using ControllerState = GLFWgamepadstate;

	class Layer;
	class EventHandler {
	public:
		bool Init();
		glm::vec2 GetCursorPos();
		void HandleEvents(std::vector<std::shared_ptr<Layer>> layerStack);
		void NewFrame();
		static float GetControllerAxis(unsigned int controllerId, unsigned int axisId);
	private:
		bool InitForController();
	};
}