#pragma once
#include <string>
#include <vector>
#include <sstream>
#include "../Scene Systems/UUID.h"
#include "../Layers/EngineAPI.h"
#include "imgui-docking/imgui.h"

namespace Pin {
	enum PinType {
		VEC2, UVEC2,
		FLOAT32, VEC3, VEC4,
		UINT8, UVEC3, UVEC4,
		PIN_TYPE_COUNT
	};

	void InitPins();
	std::string GetPinDefault(PinType type);
}

struct InputPin {
	InputPin() = delete;
	InputPin(Pin::PinType type, std::string label) : Type(type), Label(label) {}
	void DeleteConnection();

	Pin::PinType Type;
	std::string Label;
	std::string ShaderIdentifier;
	IcePick::UUID ConnectedNodeId = IcePick::UUID::Unitialised();
	unsigned int ConnectedPinIndex = 0;
};

struct OutputPin {
	OutputPin() = delete;
	OutputPin(Pin::PinType type, std::string label, std::string shaderAccessor) : Type(type), Label(label), ShaderAccessor(shaderAccessor) {}
	void DeleteConnection(IcePick::UUID connectedNode, unsigned int pinIndex);

	Pin::PinType Type;
	std::string Label;
	std::string ShaderAccessor;
	std::vector<IcePick::UUID> ConnectedNodeIds;
	std::vector<unsigned int> ConnectedPinIndices;
};

struct NodeRenderInfo {
	float NodePadding = 30.0f;
	float NodeHeaderHeight = 30.0f;
	float NodeCornerRounding = 9.0f;
	float PinYSpacing = 30.0f;
	float PinRadius = 6.0f;
	int PinSegments = 10;
	float LineThickness = 3.0f;
	unsigned int LineSegments = 40;
	ImU32 NodeBgColour = IM_COL32(20, 20, 20, 255);
	ImU32 NodeHeaderColour = IM_COL32(0xD2, 0xA1, 0x02, 255);
	ImU32 NodePinColour = IM_COL32(255, 255, 255, 255);
	ImU32 NodePinColourHovered = IM_COL32(100, 100, 255, 255);
};

class Node {
public:
	IcePick::UUID Id;
	std::vector<InputPin> InputPins;
	std::vector<OutputPin> OutputPins;
	virtual void CustomRendering(IcePick::EngineAPI engineAPI, const NodeRenderInfo& renderInfo, ImVec2 canvasScreenPos, ImVec2 canvasScrolling) {}
	virtual void Initialise(std::stringstream& ss) {}
	virtual void ParseNodeLogic(std::stringstream& ss) {}
	virtual std::string GetPinOutput(unsigned int outputPinIndex) { return ""; }
	void Unitialise();
	ImVec2 CanvasPosition;
	unsigned int NodeWidth = 150.0f;
protected:
	bool m_Initialised = false;
	std::string m_Identifier;
};
