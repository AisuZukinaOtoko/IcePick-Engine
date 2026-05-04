#pragma once
constexpr auto IP_KEYBOARD_EVENT = (0b1 << 0);
constexpr auto IP_MOUSE_EVENT = (0b1 << 1);
constexpr auto IP_CONTROLLER_EVENT = (0b1 << 2);
constexpr auto IP_EVENT_HANDLED = (0b1 << 3);

namespace IcePick {
	struct Event {
		int action = 0; // e.g press/release
		int code = 0; // key/button/axis code
		int mods = 0; // e.g shift/ctrl held
		int data = 0; // e.g controller Id
		unsigned int flags = 0; // keyboard/mouse/controller/flow flags
	};
}