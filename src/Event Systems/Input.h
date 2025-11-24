#pragma once
#include "Event.h"
#include "GLFW/glfw3.h"

namespace IcePick {
	struct KeyButton {
		bool m_Value = false;
		bool m_Checked = false;
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
		IP_CONTROLLER_D_LEFT = 0, IP_CONTROLLER_D_RIGHT, IP_CONTROLLER_D_UP,	IP_CONTROLLER_D_DOWN,
		IP_CONTROLLER_BUTTON_A, IP_CONTROLLER_BUTTON_B, IP_CONTROLLER_BUTTON_X, IP_CONTROLLER_BUTTON_Y,
		IP_CONTROLLER_BUTTON_L1, IP_CONTROLLER_BUTTON_R1, IP_CONTROLLER_BUTTON_L3, IP_CONTROLLER_BUTTON_R3,
		IP_CONTROLLER_BUTTON_START, IP_CONTROLLER_BUTTON_COUNT
	};

	enum ControllerTrigger {
		IP_CONTROLLER_TRIGGER_LEFT = 0, IP_CONTROLLER_TRIGGER_RIGHT, IP_CONTROLLER_TRIGGER_COUNT
	};

	enum ControllerAxis {
		IP_CONTROLLER_AXIS_LEFT = 0, IP_CONTROLLER_AXIS_RIGHT, IP_CONTROLLER_AXIS_COUNT
	};

	struct ControllerState {
		ControllerID Id;
		ControllerButton Buttons[IP_CONTROLLER_BUTTON_COUNT];
		ControllerTrigger Triggers[IP_CONTROLLER_TRIGGER_COUNT];
		ControllerAxis Axes[IP_CONTROLLER_AXIS_COUNT];
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
	private:
		void ProcessKeyBoardMouseEvent(Event& event);
		void ProcessControllerEvent(Event& event);
		const int m_EventTypeCount = 350;
		KeyButton m_KeysAndButtons[350];
		ControllerState m_ControllerStates[IP_CONTROLLER_COUNT];
	};
}