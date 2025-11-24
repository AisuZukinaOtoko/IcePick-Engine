#pragma once
constexpr auto IP_KEYBOARD_EVENT = (0b1 << 0);
constexpr auto IP_MOUSE_EVENT = (0b1 << 1);
constexpr auto IP_CONTROLLER_EVENT = (0b1 << 2);
constexpr auto IP_EVENT_HANDLED = (0b1 << 3);

namespace IcePick {
	struct Event {
		int action = 0;
		int code = 0;
		int mods = 0;
		unsigned int flags = 0;
	};
}