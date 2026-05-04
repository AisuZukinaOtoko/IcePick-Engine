#include "../Render Systems/Renderer.h"
#include "EventHandler.h"
#include "../LogSystem.h"
#include "../Public/Layer.h"
#include "../Utilities/DebugStatistics.h"
#include "Input.h"
#include <string>
#include <fstream>
#include <queue>

static std::vector<IcePick::ControllerButton> ControllerButtonMap;
static std::vector<unsigned int> ControllerAxisMap;
static constexpr unsigned int MaxControllerCount = 4;
static IcePick::ControllerState ControllerStates[MaxControllerCount];
static std::queue<IcePick::Event> EventQueue;

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	IcePick::Event newEvent = {action, button, mods, 0, IP_MOUSE_EVENT};
	EventQueue.push(newEvent);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	IcePick::Event newEvent = { action, key, mods, 0, IP_KEYBOARD_EVENT };
	EventQueue.push(newEvent);
}

static void joystick_callback(int joystickId, int eventCode) {
	IcePick::Event newEvent = { eventCode, 0, 0, joystickId, IP_CONTROLLER_EVENT };
	EventQueue.push(newEvent);
	if (eventCode == IP_CONNECT) {
		IP_LOG("Controller connected.", IP_WARN_LOG);
		IP_LOG(std::to_string(joystickId), IP_WARN_LOG);
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

	const unsigned int buttonMapSize = IP_CONTROLLER_BUTTON_COUNT;
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

	const unsigned int controllerAxisMapSize = IP_CONTROLLER_AXIS_COUNT;
	ControllerAxisMap.reserve(controllerAxisMapSize);
	for (int i = 0; i < controllerAxisMapSize; i++) {
		ControllerAxisMap.push_back(IP_CONTROLLER_AXIS_LEFT_X);
	}
	ControllerAxisMap[IP_CONTROLLER_TRIGGER_LEFT] = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER;
	ControllerAxisMap[IP_CONTROLLER_TRIGGER_RIGHT] = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;
	ControllerAxisMap[IP_CONTROLLER_AXIS_LEFT_X] = GLFW_GAMEPAD_AXIS_LEFT_X;
	ControllerAxisMap[IP_CONTROLLER_AXIS_LEFT_Y] = GLFW_GAMEPAD_AXIS_LEFT_Y;
	ControllerAxisMap[IP_CONTROLLER_AXIS_RIGHT_X] = GLFW_GAMEPAD_AXIS_RIGHT_X;
	ControllerAxisMap[IP_CONTROLLER_AXIS_RIGHT_Y] = GLFW_GAMEPAD_AXIS_RIGHT_Y;

	return true;
}

glm::vec2 IcePick::EventHandler::GetCursorPos() {
	double xpos, ypos;
	GLFWwindow* window = IcePickRenderer::GetRendererWindow();
	glfwGetCursorPos(window, &xpos, &ypos);
	return { xpos, ypos };
}

void IcePick::EventHandler::HandleEvents(std::vector<std::shared_ptr<Layer>> layerStack) {
	while (!EventQueue.empty()) {
		Event& event = EventQueue.front();
		for (auto& layer : layerStack) {
			if (event.flags & IP_EVENT_HANDLED)
				break;
			layer->OnEvent(event);
		}
		EventQueue.pop();
	}
}

void IcePick::EventHandler::NewFrame() {
	// Get Updated controller states
	for (unsigned int joystickId = 0; joystickId <= MaxControllerCount; joystickId++) {
		if (!glfwJoystickIsGamepad(joystickId)) {
			continue;
		}

		GLFWgamepadstate state;
		if (glfwGetGamepadState(joystickId, &state)) {
			const unsigned int axisCount = IP_CONTROLLER_AXIS_COUNT;
			for (unsigned int i = 0; i < axisCount; i++) {
				ControllerStates[joystickId].axes[i] = state.axes[i];
			}

			const unsigned int buttonCount = IP_CONTROLLER_BUTTON_COUNT;
			for (unsigned int i = 0; i < buttonCount; i++) {
				unsigned int previousFrameButtonState = ControllerStates[joystickId].buttons[i];
				unsigned int currentFrameButtonState = state.buttons[i];

				if (currentFrameButtonState != previousFrameButtonState) {
					ControllerButton controllerButton = ControllerButtonMap[i];
					Event newEvent = { currentFrameButtonState, controllerButton, 0, joystickId, IP_CONTROLLER_EVENT };
					EventQueue.push(newEvent);
				}
				ControllerStates[joystickId].buttons[i] = currentFrameButtonState;
			}
		}
	}
}

float IcePick::EventHandler::GetControllerAxis(unsigned int controllerId, unsigned int axisId) {
	return ControllerStates[controllerId].axes[ControllerAxisMap[axisId]];
}
