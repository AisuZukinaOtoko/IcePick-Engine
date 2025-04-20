#include "Input.h"

void IcePick::Input::OnEvent(Event& event) {
	if (event.action == GLFW_PRESS) {
		m_KeysAndButtons[event.code].m_Value = true;
		m_KeysAndButtons[event.code].m_Checked = false;
		m_KeysAndButtons[IcePick::IP_KEY_ANY].m_Value = true;
		m_KeysAndButtons[IcePick::IP_KEY_ANY].m_Checked = false;
	}
	else if (event.action == GLFW_RELEASE) {
		m_KeysAndButtons[event.code].m_Value = false;
		m_KeysAndButtons[event.code].m_Checked = false;
		m_KeysAndButtons[IcePick::IP_KEY_ANY].m_Value = false;
		m_KeysAndButtons[IcePick::IP_KEY_ANY].m_Checked = false;
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
