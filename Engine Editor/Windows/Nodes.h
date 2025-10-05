#pragma once
#include <string>
#include <vector>
#include "../Scene Systems/UUID.h"
#include "imgui-docking/imgui.h"

namespace Pin {
	enum PinType {
		FLOAT32, VEC3, VEC4,
		UINT8, UVEC3, UVEC4
	};
}

struct InputPin {
	InputPin() = delete;
	InputPin(Pin::PinType type, std::string label) : Type(type), Label(label) {}

	Pin::PinType Type;
	std::string Label;
	IcePick::UUID ConnectedNodeId = IcePick::UUID::Unitialised();
	unsigned int ConnectedPinIndex = 0;
};

struct OutputPin {
	OutputPin() = delete;
	OutputPin(Pin::PinType type, std::string label) : Type(type), Label(label) {}

	Pin::PinType Type;
	std::string Label;
	std::vector<IcePick::UUID> ConnectedNodeIds;
	std::vector<unsigned int> ConnectedPinIndices;
};

class Node {
public:
	IcePick::UUID Id;
	std::vector<InputPin> InputPins;
	std::vector<OutputPin> OutputPins;
	ImVec2 CanvasPosition;
};

class TextureNode : public Node {
public:
	TextureNode(IcePick::UUID textureId);
private:
	IcePick::UUID m_TextureId;
};

class BSDFNode : public Node {
public:
	BSDFNode();
};