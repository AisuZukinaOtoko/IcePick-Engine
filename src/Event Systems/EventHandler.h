#pragma once
#include "Event.h"
#include "GLFW/glfw3.h"
#include "../Vendor/glm/glm.hpp"

constexpr auto KEYBOARD_EVENT = (0b1 << 0);
constexpr auto MOUSE_EVENT = (0b1 << 1);
constexpr auto EVENT_HANDLED = (0b1 << 2);

constexpr auto IP_PRESS = GLFW_PRESS;
constexpr auto IP_RELEASE = GLFW_RELEASE;


namespace IcePick {
	class EventHandler {
	public:
		bool Init();
		glm::vec2 GetCursorPos();
		void OnEvent(Event& event);
	private:
		bool InitForController();
	};
	inline EventHandler IP_EventHandler;
}