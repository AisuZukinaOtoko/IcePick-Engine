#include "Utils.h"

std::string GetPinTypeString(Pin::PinType type) {
	switch (type) {
	case Pin::PinType::ANY:
		return "any";
	case Pin::PinType::BOOL:
		return "bool";
	case Pin::PinType::FLOAT32:
		return "float";
	case Pin::PinType::VEC2:
		return "vec2";
	case Pin::PinType::VEC3:
		return "vec3";
	case Pin::PinType::VEC4:
		return "vec4";
	}
	return std::string();
}

Pin::PinType GetPinTypeFromString(std::string& type) {
	if (type == "bool") {
		return Pin::PinType::BOOL;
	}
	if (type == "float") {
		return Pin::PinType::FLOAT32;
	}
	if (type == "vec2") {
		return Pin::PinType::VEC2;
	}
	if (type == "vec3") {
		return Pin::PinType::VEC3;
	}
	if (type == "vec4") {
		return Pin::PinType::VEC4;
	}
	return Pin::PinType::ANY;
}
