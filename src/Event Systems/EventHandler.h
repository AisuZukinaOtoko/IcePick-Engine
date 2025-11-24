#pragma once
#include "Event.h"
#include "GLFW/glfw3.h"
#include "../Vendor/glm/glm.hpp"

constexpr auto IP_PRESS = GLFW_PRESS;
constexpr auto IP_RELEASE = GLFW_RELEASE;

namespace IcePick {
	class EventHandler {
	public:
		bool Init();
		glm::vec2 GetCursorPos();
		void OnEvent(Event& event);
		void NewFrame();
	private:
		bool InitForController();
	};
	inline EventHandler IP_EventHandler;
}