#include "../Render Systems/Renderer.h"
#include "EventHandler.h"
#include "../LogSystem.h"
#include "../Layers/Layer.h"
#include <string>
#include <fstream>


static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	IcePick::Event newEvent = {action, button, mods, MOUSE_EVENT};
	IcePick::IP_EventHandler.OnEvent(newEvent);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	IcePick::Event newEvent = { action, key, mods, KEYBOARD_EVENT };
	IcePick::IP_EventHandler.OnEvent(newEvent);
}

static void joystick_callback(int jid, int eventCode) {
	if (eventCode == GLFW_CONNECTED) {
		// The joystick was connected
		IP_LOG("Controller connected. Not yet supported.", IP_WARN_LOG);
	}
	else if (eventCode == GLFW_DISCONNECTED) {
		// The joystick was disconnected
	}
}


bool IcePick::EventHandler::Init(){
	GLFWwindow* window = IcePickRenderer::GetRendererWindow();
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetJoystickCallback(joystick_callback);
	InitForController();
	return true;
}

bool IcePick::EventHandler::InitForController() {
	std::ifstream file("res/Data/gamecontrollerdb.txt");
	if (!file.is_open()) {
		IP_LOG("Failed to load controller database.", IP_WARN_LOG);
		IP_LOG("Controller mappings may be incorrect.", IP_WARN_LOG);
		return false;
	}

	std::string fileString((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	glfwUpdateGamepadMappings(fileString.c_str());
	return true;
}

glm::vec2 IcePick::EventHandler::GetCursorPos() {
	double xpos, ypos;
	GLFWwindow* window = IcePickRenderer::GetRendererWindow();
	glfwGetCursorPos(window, &xpos, &ypos);
	return { xpos, ypos };
}

void IcePick::EventHandler::OnEvent(Event& event) {
	for (auto& layer : IcePick::IP_LayerStack.m_Layers) {
		if (event.flags & EVENT_HANDLED)
			break;

		layer->OnEvent(event);
	}
}

