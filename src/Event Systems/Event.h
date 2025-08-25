#pragma once

namespace IcePick {
	struct Event {
		int action = 0;
		int code = 0;
		int mods = 0;
		unsigned int flags = 0;
	};
}