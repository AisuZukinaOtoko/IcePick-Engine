#include "../Render Systems/Renderer.h"
#include "EventHandler.h"
#include "../LogSystem.h"
#include "../Layers/Layer.h"
#include "../Utilities/DebugStatistics.h"
#include "Input.h"
#include <string>
#include <fstream>

static std::vector<IcePick::ControllerButton> ControllerButtonMap;

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	IcePick::Event newEvent = {action, button, mods, 0, IP_MOUSE_EVENT};
	IcePick::IP_EventHandler.OnEvent(newEvent);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	IcePick::Event newEvent = { action, key, mods, 0, IP_KEYBOARD_EVENT };
	IcePick::IP_EventHandler.OnEvent(newEvent);
}

static void joystick_callback(int joystickId, int eventCode) {
	IcePick::Event newEvent = { eventCode, 0, 0, joystickId, IP_CONTROLLER_EVENT };
	IcePick::IP_EventHandler.OnEvent(newEvent);
	if (eventCode == IP_CONNECT) {
		IP_LOG("Controller connected. Not yet supported.", IP_WARN_LOG);
	}
	else if (eventCode == IP_DISCONNECT) {
		IP_LOG("Controller disconnected.", IP_WARN_LOG);
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
	std::ifstream file("res/data/gamecontrollerdb.txt");
	if (!file.is_open()) {
		IP_LOG("Failed to load controller database.", IP_WARN_LOG);
		IP_LOG("Controller mappings may be incorrect.", IP_WARN_LOG);
		return false;
	}

	std::string fileString((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	glfwUpdateGamepadMappings(fileString.c_str());

	const unsigned int buttonMapSize = 15;
	ControllerButtonMap.reserve(buttonMapSize);
	for (int i = 0; i < buttonMapSize; i++) {
		ControllerButtonMap.push_back(IP_CONTROLLER_BUTTON_CROSS);
	}

	ControllerButtonMap[GLFW_GAMEPAD_BUTTON_CROSS] = IP_CONTROLLER_BUTTON_CROSS;
	ControllerButtonMap[GLFW_GAMEPAD_BUTTON_CIRCLE] = IP_CONTROLLER_BUTTON_CIRCLE;
	ControllerButtonMap[GLFW_GAMEPAD_BUTTON_SQUARE] = IP_CONTROLLER_BUTTON_SQUARE;
	ControllerButtonMap[GLFW_GAMEPAD_BUTTON_TRIANGLE] = IP_CONTROLLER_BUTTON_TRIANGLE;
	ControllerButtonMap[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] = IP_CONTROLLER_BUTTON_L1;
	ControllerButtonMap[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] = IP_CONTROLLER_BUTTON_R1;
	ControllerButtonMap[GLFW_GAMEPAD_BUTTON_BACK] = IP_CONTROLLER_BUTTON_SELECT;
	ControllerButtonMap[GLFW_GAMEPAD_BUTTON_START] = IP_CONTROLLER_BUTTON_START;
	ControllerButtonMap[GLFW_GAMEPAD_BUTTON_GUIDE] = IP_CONTROLLER_BUTTON_PS;
	ControllerButtonMap[GLFW_GAMEPAD_BUTTON_LEFT_THUMB] = IP_CONTROLLER_BUTTON_L3;
	ControllerButtonMap[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB] = IP_CONTROLLER_BUTTON_R3;
	ControllerButtonMap[GLFW_GAMEPAD_BUTTON_DPAD_UP] = IP_CONTROLLER_DPAD_UP;
	ControllerButtonMap[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] = IP_CONTROLLER_DPAD_RIGHT;
	ControllerButtonMap[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] = IP_CONTROLLER_DPAD_DOWN;
	ControllerButtonMap[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] = IP_CONTROLLER_DPAD_LEFT;
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
		if (event.flags & IP_EVENT_HANDLED)
			break;

		layer->OnEvent(event);
	}
}

void IcePick::EventHandler::NewFrame() {
	// Get Updated controller states
	for (unsigned int joystickId = 0; joystickId <= m_MaxControllerCount; joystickId++) {
		if (!glfwJoystickIsGamepad(joystickId)) {
			continue;
		}

		GLFWgamepadstate state;
		if (glfwGetGamepadState(joystickId, &state)) {
			const unsigned int axisCount = 6;
			for (unsigned int i = 0; i < axisCount; i++) {
				m_ControllerStates[joystickId].axes[i] = state.axes[i];
				IP_CORE_PROFILE_LOG("Some axis", state.axes[i]);
				IP_CORE_PROFILE_POP();
			}
			

			IP_CORE_PROFILE_LOG("Dpad Up", state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP]);
			IP_CORE_PROFILE_POP();

			const unsigned int buttonCount = 15;
			for (unsigned int i = 0; i < axisCount; i++) {
				unsigned int previousFrameButtonState = m_ControllerStates[joystickId].buttons[i];
				unsigned int currentFrameButtonState = state.buttons[i];

				if (currentFrameButtonState != previousFrameButtonState) {
					ControllerButton controllerButton = ControllerButtonMap[i];
					Event newEvent = { currentFrameButtonState, controllerButton, 0, joystickId, IP_CONTROLLER_EVENT };
					OnEvent(newEvent);
				}
				m_ControllerStates[joystickId].buttons[i] = currentFrameButtonState;
			}
		}
	}
}