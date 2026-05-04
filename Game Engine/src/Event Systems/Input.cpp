#include "../Render Systems/Renderer.h"
#include "Input.h"
#include "EventHandler.h"
#include "../Utilities/Assert.h"

void IcePick::Input::OnEvent(Event& event) {
	if (event.flags & (IP_KEYBOARD_EVENT | IP_MOUSE_EVENT)) {
		ProcessKeyBoardMouseEvent(event);
		m_CurrentInputType = InputPeripheral::IP_KEYBOARD_MOUSE;
	}
	else if (event.flags & IP_CONTROLLER_EVENT) {
		ProcessControllerEvent(event);
		m_CurrentInputType = InputPeripheral::IP_CONTROLLER;
		m_CurrentControllerId = static_cast<ControllerID>(event.data);
	}
}

void IcePick::Input::ClearEvents(){
	for (int i = 0; i < m_EventTypeCount; i++) {
		m_KeysAndButtons[i].m_Value = m_KeysAndButtons[i].m_Checked = false;
	}
}

bool IcePick::Input::IsKeyPressed(IcePick::InputKey key) {
	bool result = m_KeysAndButtons[key].m_Value && !m_KeysAndButtons[key].m_Checked;
	m_KeysAndButtons[key].m_Checked = true;
	return result;
}

bool IcePick::Input::IsKeyHeld(IcePick::InputKey key){
	return m_KeysAndButtons[key].m_Value;
}

bool IcePick::Input::IsKeyReleased(IcePick::InputKey key) {
	return !m_KeysAndButtons[key].m_Value;
}

bool IcePick::Input::IsMouseButtonPressed(IcePick::MouseButton button){
	bool result = m_KeysAndButtons[button].m_Value && !m_KeysAndButtons[button].m_Checked;
	m_KeysAndButtons[button].m_Checked = true;
	return result;
}

bool IcePick::Input::IsMouseButtonHeld(IcePick::MouseButton button){
	return m_KeysAndButtons[button].m_Value;
}

bool IcePick::Input::IsMouseButtonReleased(IcePick::MouseButton button) {
	return !m_KeysAndButtons[button].m_Value;
}

void IcePick::Input::ProcessKeyBoardMouseEvent(Event& event) {
	if (event.action == IP_PRESS) {
		m_KeysAndButtons[event.code].m_Value = true;
		m_KeysAndButtons[event.code].m_Checked = false;
		m_KeysAndButtons[IcePick::IP_KEY_ANY].m_Value = true;
		m_KeysAndButtons[IcePick::IP_KEY_ANY].m_Checked = false;
	}
	else if (event.action == IP_RELEASE) {
		m_KeysAndButtons[event.code].m_Value = false;
		m_KeysAndButtons[event.code].m_Checked = false;
		m_KeysAndButtons[IcePick::IP_KEY_ANY].m_Value = false;
		m_KeysAndButtons[IcePick::IP_KEY_ANY].m_Checked = false;
	}
}

void IcePick::Input::ProcessControllerEvent(Event& event) {
	IP_ASSERT(event.data < IP_CONTROLLER_COUNT, "Invalid controller ID");
	if (event.action == IP_CONNECT) {
		m_ControllerStates[event.data].ControllerConnected = true;
	}
	else if (event.action == IP_DISCONNECT) {
		m_ControllerStates[event.data].ControllerConnected = false;
	}
	else {
		m_ControllerStates[event.data].ControllerButtons[event.code].m_Value = event.action;
		m_ControllerStates[event.data].ControllerButtons[event.code].m_Checked = false;
	}	
}

bool IcePick::Input::IsControllerButtonPressed(ControllerID controllerId, ControllerButton button) {
	IP_ASSERT(controllerId < IP_CONTROLLER_COUNT, "Invalid controller ID");
	IP_ASSERT(button < IP_CONTROLLER_BUTTON_COUNT, "Invalid controller button");
	bool result = m_ControllerStates[(unsigned int)controllerId].ControllerButtons[(unsigned int)button].m_Value && !m_ControllerStates[(unsigned int)controllerId].ControllerButtons[(unsigned int)button].m_Checked;
	m_ControllerStates[controllerId].ControllerButtons[button].m_Checked = true;
	return result;
}

bool IcePick::Input::IsControllerButtonHeld(ControllerID controllerId, ControllerButton button) {
	return m_ControllerStates[controllerId].ControllerButtons[button].m_Value;
}

bool IcePick::Input::IsControllerButtonReleased(ControllerID controllerId, ControllerButton button) {
	return !IsControllerButtonHeld(controllerId, button);
}

float IcePick::Input::GetControllerTriggerValue(ControllerID controllerId, ControllerTrigger trigger) {
	return IcePick::EventHandler::GetControllerAxis(controllerId, trigger);
}

float IcePick::Input::GetControllerAxisValue(ControllerID controllerId, ControllerAxis axis) {
	return IcePick::EventHandler::GetControllerAxis(controllerId, axis);
}

glm::vec2 IcePick::Input::GetLookActionAxes() {
	glm::vec2 lookAxes{ 0.0f, 0.0f };
	if (m_CurrentInputType == InputPeripheral::IP_KEYBOARD_MOUSE) {
		lookAxes = IcePickRenderer::GetMouseDelta();
	}
	else if (m_CurrentInputType == InputPeripheral::IP_CONTROLLER) {
		float controllerSensitivityMultiplier = 10.0f;
		lookAxes.x = GetControllerAxisValue(m_CurrentControllerId, IP_CONTROLLER_AXIS_RIGHT_X);
		lookAxes.y = GetControllerAxisValue(m_CurrentControllerId, IP_CONTROLLER_AXIS_RIGHT_Y);
		lookAxes *= controllerSensitivityMultiplier;
	}

	return lookAxes;
}

glm::vec2 IcePick::Input::GetWalkActionAxes() {
	glm::vec2 walkActionAxes{ 0.0f, 0.0f };

	if (m_CurrentInputType == InputPeripheral::IP_KEYBOARD_MOUSE) {
		bool wPressed = IsKeyHeld(IP_KEY_W);
		bool aPressed = IsKeyHeld(IP_KEY_A);
		bool sPressed = IsKeyHeld(IP_KEY_S);
		bool dPressed = IsKeyHeld(IP_KEY_D);
		walkActionAxes = { (float)dPressed - (float)aPressed, (float)wPressed - (float)sPressed };
	}
	else if (m_CurrentInputType == InputPeripheral::IP_CONTROLLER) {
		walkActionAxes.x = GetControllerAxisValue(m_CurrentControllerId, IP_CONTROLLER_AXIS_LEFT_X);
		walkActionAxes.y = -1 * GetControllerAxisValue(m_CurrentControllerId, IP_CONTROLLER_AXIS_LEFT_Y);
	}

	return walkActionAxes;
}
