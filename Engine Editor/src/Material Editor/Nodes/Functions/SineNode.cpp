#include "SineNode.h"
#include "../Utils.h"

SineNode::SineNode() {
	InputPins.emplace_back(Pin::ANY, "A");
	OutputPins.emplace_back(Pin::ANY, "sin(A)", "");
	m_NodeType = "sine";
	NodeName = "Sine";
	nodeCanBeParamterized = false;
	NodeHeaderColour = ImU32(0xFF1B9F47);
}

void SineNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	OutputPins[0].Type = InputPins[0].ConnectedPinType;
	m_Identifier = "node_" + std::to_string(Id);
	std::string& sx = InputPins[0].ShaderIdentifier;
	std::string type = GetPinTypeString(InputPins[0].ConnectedPinType);
	ss << type << " " << m_Identifier << " = sin(" << sx << ");\n";
	m_Initialised = true;
}

void SineNode::ParseNodeLogic(std::stringstream& ss) {

}

bool SineNode::NodeStateValid() {
	bool pinsAreTypeAny = (InputPins[0].ConnectedPinType == Pin::PinType::ANY);
	return !pinsAreTypeAny;
}

std::string SineNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}
