#pragma once
#include "Event.h"
#include "GLFW/glfw3.h"

namespace IcePick {
	struct Button {
		bool m_Value = false;
		bool m_Checked = false;
	};

	enum InputPeripheral {
		IP_KEYBOARD_MOUSE, IP_CONTROLLER, IP_PERIPHERAL_COUNT
	};

	enum InputKey {
		IP_KEY_SPACE = GLFW_KEY_SPACE,

		IP_KEY_0 = GLFW_KEY_0, IP_KEY_1, IP_KEY_2, IP_KEY_3, IP_KEY_4, IP_KEY_5,
		IP_KEY_6, IP_KEY_7, IP_KEY_8, IP_KEY_9,

		IP_KEY_A = GLFW_KEY_A, IP_KEY_B, IP_KEY_C, IP_KEY_D, IP_KEY_E, IP_KEY_F,
		IP_KEY_G, IP_KEY_H, IP_KEY_I, IP_KEY_J, IP_KEY_K, IP_KEY_L, IP_KEY_M, IP_KEY_N,
		IP_KEY_O, IP_KEY_P, IP_KEY_Q, IP_KEY_R, IP_KEY_S, IP_KEY_T, IP_KEY_U, IP_KEY_V,
		IP_KEY_W, IP_KEY_X, IP_KEY_Y, IP_KEY_Z,

		IP_KEY_ESC = GLFW_KEY_ESCAPE, IP_KEY_ENTER, IP_KEY_TAB,

		IP_KEY_LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT, IP_KEY_LEFT_CTRL, IP_KEY_LEFT_ALT, IP_KEY_LEFT_SUPER,
		IP_KEY_RIGHT_SHIFT, IP_KEY_RIGHT_CTRL, IP_KEY_RIGHT_ALT,

		IP_KEY_ANY
	};
	enum MouseButton {
		IP_MOUSE_LEFT = GLFW_MOUSE_BUTTON_LEFT, IP_MOUSE_RIGHT, IP_MOUSE_MIDDLE
		, IP_MOUSE_4, IP_MOUSE_5, IP_MOUSE_ANY
	};

	enum ControllerID {
		IP_CONTROLLER_1 = 0, IP_CONTROLLER_2, IP_CONTROLLER_3, IP_CONTROLLER_4,
		IP_CONTROLLER_5, IP_CONTROLLER_6, IP_CONTROLLER_7, IP_CONTROLLER_8, IP_CONTROLLER_COUNT
	};

	enum ControllerButton {
		IP_CONTROLLER_BUTTON_CROSS = 0, IP_CONTROLLER_BUTTON_CIRCLE, IP_CONTROLLER_BUTTON_SQUARE, IP_CONTROLLER_BUTTON_TRIANGLE,
		IP_CONTROLLER_DPAD_UP, IP_CONTROLLER_DPAD_RIGHT, IP_CONTROLLER_DPAD_DOWN, IP_CONTROLLER_DPAD_LEFT,
		IP_CONTROLLER_BUTTON_L1, IP_CONTROLLER_BUTTON_R1, IP_CONTROLLER_BUTTON_L3, IP_CONTROLLER_BUTTON_R3,
		IP_CONTROLLER_BUTTON_START, IP_CONTROLLER_BUTTON_SELECT, IP_CONTROLLER_BUTTON_PS,
		IP_CONTROLLER_BUTTON_COUNT
	};

	enum ControllerTrigger {
		IP_CONTROLLER_TRIGGER_LEFT = 0, IP_CONTROLLER_TRIGGER_RIGHT, IP_CONTROLLER_TRIGGER_COUNT
	};

	enum ControllerAxis {
		IP_CONTROLLER_AXIS_LEFT_X = IP_CONTROLLER_TRIGGER_COUNT, IP_CONTROLLER_AXIS_LEFT_Y, IP_CONTROLLER_AXIS_RIGHT_X,
		IP_CONTROLLER_AXIS_RIGHT_Y, IP_CONTROLLER_AXIS_COUNT
	};

	struct ControllerInputState {
		Button ControllerButtons[IP_CONTROLLER_BUTTON_COUNT];
		ControllerTrigger Triggers[IP_CONTROLLER_TRIGGER_COUNT];
		ControllerAxis Axes[IP_CONTROLLER_AXIS_COUNT];
		ControllerID Id;
		bool ControllerConnected = false;
	};

	class Input {
	public:
		Input() = default;
		~Input() = default;
		void OnEvent(Event& event);

		void ClearEvents();
		bool IsKeyPressed(InputKey key);
		bool IsKeyHeld(InputKey key);
		bool IsKeyReleased(InputKey key);
		bool IsMouseButtonPressed(MouseButton button);
		bool IsMouseButtonHeld(MouseButton button);
		bool IsMouseButtonReleased(MouseButton button);

		bool IsControllerButtonPressed(ControllerID controllerId, ControllerButton button);
		bool IsControllerButtonHeld(ControllerID controllerId, ControllerButton button);
		bool IsControllerButtonReleased(ControllerID controllerId, ControllerButton button);

		float GetControllerTriggerValue(ControllerID controllerId, ControllerTrigger trigger);
		float GetControllerAxisValue(ControllerID controllerId, ControllerAxis axis);

		glm::vec2 GetLookActionAxes();
		glm::vec2 GetWalkActionAxes();
	private:
		void ProcessKeyBoardMouseEvent(Event& event);
		void ProcessControllerEvent(Event& event);
		InputPeripheral m_CurrentInputType = InputPeripheral::IP_KEYBOARD_MOUSE;
		ControllerID m_CurrentControllerId = ControllerID::IP_CONTROLLER_1;

		const int m_EventTypeCount = 350;
		Button m_KeysAndButtons[350];
		ControllerInputState m_ControllerStates[IP_CONTROLLER_COUNT];
	};
}