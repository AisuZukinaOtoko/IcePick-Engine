#pragma once
#include "NodeBase.h"
#include <string>

std::string GetPinTypeString(Pin::PinType type);

Pin::PinType GetPinTypeFromString(std::string& type);