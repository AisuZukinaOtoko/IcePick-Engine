#pragma once
#include "UUID.h"

namespace IcePick {
	struct ScriptSlot {
		UUID ScriptId = UUID::Unitialised();
		bool Active = false;
	};
}