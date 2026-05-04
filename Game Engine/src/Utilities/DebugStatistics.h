#pragma once
#include <vector>
#include <string>
#include "Timer.h"

struct DebugCapture {
	enum CaptureType {
		VALUE = 0,
		TIME_PROFILE
	};
	DebugCapture(std::string name) : PropertyName(name), Type(TIME_PROFILE) {};
	DebugCapture(std::string name, std::string value) : PropertyName(name), PropertyValue(value), Type(VALUE) {};
	CaptureType Type;
	ManualTimer Timer;
	std::string PropertyName;
	std::string PropertyValue;
	std::vector<DebugCapture> ChildCaptures;
};

void IP_CORE_PROFILE_BEGIN(std::string propertyName);
//void IP_CORE_PROFILE_PUSH(std::string propertyName);
void IP_CORE_PROFILE_POP();

void IP_CORE_PROFILE_CLEAR();

void IP_CORE_PROFILE_LOG(std::string propertyName, int propertyValue);
void IP_CORE_PROFILE_LOG(std::string propertyName, float propertyValue);
void IP_CORE_PROFILE_LOG(std::string propertyName, std::string propertyValue);

void IP_CORE_PROFILE_CAPTURE();

const DebugCapture& IP_GET_CORE_PROFILE();