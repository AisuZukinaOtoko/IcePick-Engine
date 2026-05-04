#pragma once
#include <sol/sol.hpp>

namespace IcePick {
	struct Script {
		sol::environment ScriptEnvironment = sol::nil;
		sol::protected_function OnCreateFunction = sol::nil;
		sol::protected_function OnUpdateFunction = sol::nil;
		sol::protected_function OnDestroyFunction = sol::nil;

		bool IsValid = false;
	};
}