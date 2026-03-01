#include "LerpNode.h"
#include "../Utils.h"

LerpNode::LerpNode() {
	InputPins.emplace_back(Pin::ANY, "A");
	InputPins.emplace_back(Pin::ANY, "B");
	InputPins.emplace_back(Pin::ANY, "Threshold");
	OutputPins.emplace_back(Pin::ANY, "Out", "");
	m_NodeType = "lerp";
	NodeName = "Lerp";
	nodeCanBeParamterized = false;
	NodeHeaderColour = ImU32(0xFF1B9F47);
}

void LerpNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	OutputPins[0].Type = InputPins[0].ConnectedPinType;
	m_Identifier = "node_" + std::to_string(Id);
	std::string& sa = InputPins[0].ShaderIdentifier;
	std::string& sb = InputPins[1].ShaderIdentifier;
	std::string& st = InputPins[2].ShaderIdentifier;
	std::string type = GetPinTypeString(InputPins[0].ConnectedPinType);
	ss << type << " " << m_Identifier << " = mix(" << sa<< ", " << sb << ", " << st << ");\n";
	m_Initialised = true;
}

void LerpNode::ParseNodeLogic(std::stringstream& ss) {

}

bool LerpNode::NodeStateValid() {
	bool pinsAreTypeAny = (InputPins[0].ConnectedPinType == Pin::PinType::ANY) || (InputPins[1].ConnectedPinType == Pin::PinType::ANY);
	bool pinTypesMatch = (InputPins[0].ConnectedPinType == InputPins[1].ConnectedPinType);
	bool thresholdPinIsValid = (InputPins[2].ConnectedPinType == Pin::PinType::FLOAT32) || (InputPins[0].ConnectedPinType == InputPins[2].ConnectedPinType);
	return !pinsAreTypeAny && pinTypesMatch && thresholdPinIsValid;
}

std::string LerpNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}
