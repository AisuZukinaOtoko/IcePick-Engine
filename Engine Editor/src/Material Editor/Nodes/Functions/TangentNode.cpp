#include "TangentNode.h"
#include "../Utils.h"

TangentNode::TangentNode() {
	InputPins.emplace_back(Pin::ANY, "A");
	OutputPins.emplace_back(Pin::ANY, "tan(A)", "");
	m_NodeType = "tangent";
	NodeName = "Tangent";
	nodeCanBeParamterized = false;
	NodeHeaderColour = ImU32(0xFF1B9F47);
}

void TangentNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	OutputPins[0].Type = InputPins[0].ConnectedPinType;
	m_Identifier = "node_" + std::to_string(Id);
	std::string& sx = InputPins[0].ShaderIdentifier;
	std::string type = GetPinTypeString(InputPins[0].ConnectedPinType);
	ss << type << " " << m_Identifier << " = tan(" << sx << ");\n";
	m_Initialised = true;
}

void TangentNode::ParseNodeLogic(std::stringstream& ss) {

}

bool TangentNode::NodeStateValid() {
	bool pinsAreTypeAny = (InputPins[0].ConnectedPinType == Pin::PinType::ANY);
	return !pinsAreTypeAny;
}

std::string TangentNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}
