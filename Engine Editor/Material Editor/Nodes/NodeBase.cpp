#include "Nodebase.h"
#include "../Utilities/Assert.h"

static std::string pinDefaults[Pin::PIN_TYPE_COUNT];

void Pin::InitPins() {
	pinDefaults[ANY] = "missing_value";
	pinDefaults[VEC2] = "vec2(0.0f)";
	pinDefaults[UVEC2] = "uvec2(0)";
	pinDefaults[FLOAT32] = "0.0f";
	pinDefaults[VEC3] = "vec3(0.0f)";
	pinDefaults[VEC4] = "vec4(0.0f)";
	pinDefaults[UINT8] = "0";
	pinDefaults[UVEC3] = "uvec3(0)";
	pinDefaults[UVEC4] = "uvec4(0)";
}

std::string Pin::GetPinDefault(PinType type) {
	IP_ASSERT(type != Pin::PIN_TYPE_COUNT, "Invalid pin type.");
	return pinDefaults[type];
}

void InputPin::DeleteConnection() {
	ConnectedNodeId = IcePick::UUID::Unitialised();
	ConnectedPinIndex = 0;
	ConnectedPinType = Pin::PinType::ANY;
}

void OutputPin::DeleteConnection(IcePick::UUID connectedNode, unsigned int pinIndex) {
	int index = -1;
	for (int i = 0; i < ConnectedNodeIds.size(); i++) {
		if ((ConnectedNodeIds[i] == connectedNode) && (ConnectedPinIndices[i] == pinIndex))
			index = i;
	}

	if (index == -1)
		return;

	ConnectedNodeIds[index] = ConnectedNodeIds.back();
	ConnectedPinIndices[index] = ConnectedPinIndices.back();
	ConnectedNodeIds.pop_back();
	ConnectedPinIndices.pop_back();
}

void Node::Unitialise() {
	m_Initialised = false;
}
