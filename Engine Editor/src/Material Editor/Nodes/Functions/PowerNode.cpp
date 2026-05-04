#include "PowerNode.h"
#include "../Utils.h"

PowerNode::PowerNode() {
	InputPins.emplace_back(Pin::ANY, "A");
	InputPins.emplace_back(Pin::FLOAT32, "B");
	OutputPins.emplace_back(Pin::ANY, "A^B", "");
	m_NodeType = "power";
	NodeName = "Power";
	nodeCanBeParamterized = false;
	NodeHeaderColour = ImU32(0xFF1B9F47);
}

void PowerNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	OutputPins[0].Type = InputPins[0].ConnectedPinType;
	m_Identifier = "node_" + std::to_string(Id);
	std::string& s1 = InputPins[0].ShaderIdentifier;
	std::string& s2 = InputPins[1].ShaderIdentifier;
	std::string type = GetPinTypeString(InputPins[0].ConnectedPinType);
	ss << type << " " << m_Identifier << " = pow(" << s1 << ", " << s2 <<");\n";
	m_Initialised = true;
}

void PowerNode::ParseNodeLogic(std::stringstream& ss) {

}

bool PowerNode::NodeStateValid() {
	bool pinsAreTypeAny = (InputPins[0].ConnectedPinType == Pin::PinType::ANY) || (InputPins[1].ConnectedPinType == Pin::PinType::ANY);
	bool pinTypesMatch = (InputPins[0].ConnectedPinType == InputPins[1].ConnectedPinType);
	return (!pinsAreTypeAny && pinTypesMatch);
}

std::string PowerNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}
