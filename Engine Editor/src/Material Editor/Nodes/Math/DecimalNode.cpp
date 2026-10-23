#include "DecimalNode.h"
#include "../Utils.h"

DecimalNode::DecimalNode() {
	InputPins.emplace_back(Pin::ANY, "A");
	OutputPins.emplace_back(Pin::ANY, "Decimal", "");
	m_NodeType = "decimal";
	NodeName = "Decimal";
	nodeCanBeParamterized = false;
}

void DecimalNode::ParseLogic(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_NodeVisited)
		return;

	OutputPins[0].Type = InputPins[0].ConnectedPinType;
	m_Identifier = "node_" + std::to_string(Id);
	std::string& sx = InputPins[0].ShaderIdentifier;
	std::string type = GetPinTypeString(InputPins[0].ConnectedPinType);
	ss << type << " " << m_Identifier << " = fract(" << sx << ");\n";
	m_NodeVisited = true;
}

bool DecimalNode::NodeStateValid() {
	bool pinsAreTypeAny = (InputPins[0].ConnectedPinType == Pin::PinType::ANY);
	return !pinsAreTypeAny;
}

std::string DecimalNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}
