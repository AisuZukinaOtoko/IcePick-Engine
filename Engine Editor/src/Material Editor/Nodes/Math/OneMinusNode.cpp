#include "OneMinusNode.h"
#include "../Utils.h"

OneMinusNode::OneMinusNode() {
	InputPins.emplace_back(Pin::ANY, "A");
	OutputPins.emplace_back(Pin::ANY, "1-A", "");
	NodeName = "One Minus";
	m_NodeType = "oneMinus";
	m_Identifier = "node_" + std::to_string(Id);
}

void OneMinusNode::ParseLogic(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_NodeVisited)
		return;

	OutputPins[0].Type = InputPins[0].ConnectedPinType;
	std::string& sx = InputPins[0].ShaderIdentifier;
	std::string type = GetPinTypeString(InputPins[0].ConnectedPinType);
	ss << type << " " << m_Identifier << " = " << type + "(1.0f) - " << sx << ";\n";
	m_NodeVisited = true;
}

bool OneMinusNode::NodeStateValid() {
	bool pinsAreTypeAny = (InputPins[0].ConnectedPinType == Pin::PinType::ANY);
	return !pinsAreTypeAny;
}

std::string OneMinusNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}
