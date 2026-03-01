#include "SmoothstepNode.h"
#include "../Utils.h"

SmoothstepNode::SmoothstepNode() {
	InputPins.emplace_back(Pin::ANY, "In");
	InputPins.emplace_back(Pin::ANY, "Edge 1");
	InputPins.emplace_back(Pin::ANY, "Edge 2");
	OutputPins.emplace_back(Pin::ANY, "Out", "");
	m_NodeType = "smoothstep";
	NodeName = "Smoothstep";
	nodeCanBeParamterized = false;
	NodeHeaderColour = ImU32(0xFF1B9F47);
}

void SmoothstepNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	OutputPins[0].Type = InputPins[0].ConnectedPinType;
	m_Identifier = "node_" + std::to_string(Id);
	std::string& sI = InputPins[0].ShaderIdentifier;
	std::string& sE1 = InputPins[1].ShaderIdentifier;
	std::string& sE2 = InputPins[2].ShaderIdentifier;
	std::string type = GetPinTypeString(InputPins[0].ConnectedPinType);
	ss << type << " " << m_Identifier << " = smoothstep(" << sE1 << ", " << sE2 << ", " << sI << ");\n";
	m_Initialised = true;
}

void SmoothstepNode::ParseNodeLogic(std::stringstream& ss) {

}

bool SmoothstepNode::NodeStateValid() {
	bool inPinIsTypeAny = InputPins[0].ConnectedPinType == Pin::PinType::ANY;
	bool edge1PinIsValid = (InputPins[1].ConnectedPinType != Pin::PinType::ANY) && ((InputPins[0].ConnectedPinType == InputPins[1].ConnectedPinType) || (InputPins[1].ConnectedPinType == Pin::PinType::FLOAT32));
	bool edge2PinIsValid = (InputPins[2].ConnectedPinType != Pin::PinType::ANY) && ((InputPins[0].ConnectedPinType == InputPins[2].ConnectedPinType) || (InputPins[2].ConnectedPinType == Pin::PinType::FLOAT32));
	bool edgePinsValid = edge1PinIsValid && edge2PinIsValid;
	bool edgePinTypesMatch = InputPins[1].ConnectedPinType == InputPins[2].ConnectedPinType;
	return !inPinIsTypeAny && edgePinsValid && edgePinTypesMatch;
}

std::string SmoothstepNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}
