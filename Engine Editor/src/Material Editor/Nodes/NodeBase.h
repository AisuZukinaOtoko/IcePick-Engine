#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include "Scene Systems/UUID.h"
#include <imgui-docking/imgui.h>
#include "File Systems/MaterialBase.h"

namespace Pin {
	enum PinType {
		ANY, BOOL, VEC2, UVEC2,
		FLOAT32, VEC3, VEC4,
		UINT8, UVEC3, UVEC4,
		PIN_TYPE_COUNT
	};

	void InitPins();
	std::string GetPinDefault(PinType type);
}

namespace IcePick {
	class EngineAPI;
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
	Pin::PinType ConnectedPinType = Pin::PinType::ANY;
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
	float NodeCornerRounding = 5.0f;
	float PinYSpacing = 30.0f;
	float PinRadius = 9.0f;
	int PinSegments = 15;
	float LineThickness = 3.0f;
	unsigned int LineSegments = 40;
	float LabelPadding = 5.0f;
	ImU32 LabelColour = IM_COL32(255, 255, 255, 255);
	ImU32 NodeBgColour = IM_COL32(20, 20, 20, 255);
	ImU32 NodePinColour = IM_COL32(255, 255, 255, 255);
	ImU32 NodePinColourHovered = IM_COL32(100, 100, 255, 255);
};

class Node {
public:
	IcePick::UUID Id;
	std::string NodeName;
	std::vector<InputPin> InputPins;
	std::vector<OutputPin> OutputPins;
	virtual void CustomRendering(IcePick::EngineAPI& engineAPI, std::filesystem::path& dropAssetPath, const NodeRenderInfo& renderInfo, ImVec2 canvasScreenPos, ImVec2 canvasScrolling) {}
	virtual void Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {}
	virtual void ParseNodeLogic(std::stringstream& ss) {}
	virtual std::string GetPinOutput(unsigned int outputPinIndex) { return ""; }
	virtual bool NodeStateValid() { return true; }
	void Unitialise();
	std::string GetNodeType() { return m_NodeType; }
	ImVec2 CanvasPosition;
	unsigned int NodeWidth = 150.0f;
	ImU32 NodeHeaderColour = IM_COL32(0xD2, 0xA1, 0x02, 255);

	bool MaterialBaseStateChanged = false;
	bool MaterialInstanceStateChanged = false;

	bool nodeIsParameter = false;
	bool nodeCanBeParamterized = false;
protected:
	bool m_Initialised = false;
	std::string m_Identifier;
	std::string m_NodeType = "base";
};

typedef std::vector<std::shared_ptr<Node>> Graph;
