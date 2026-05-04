#include "CosineNode.h"
#include "../Utils.h"

CosineNode::CosineNode() {
	InputPins.emplace_back(Pin::ANY, "A");
	OutputPins.emplace_back(Pin::ANY, "cos(A)", "");
	m_NodeType = "cosine";
	NodeName = "Cosine";
	nodeCanBeParamterized = false;
	NodeHeaderColour = ImU32(0xFF1B9F47);
}

void CosineNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	OutputPins[0].Type = InputPins[0].ConnectedPinType;
	m_Identifier = "node_" + std::to_string(Id);
	std::string& sx = InputPins[0].ShaderIdentifier;
	std::string type = GetPinTypeString(InputPins[0].ConnectedPinType);
	ss << type << " " << m_Identifier << " = cos(" << sx << ");\n";
	m_Initialised = true;
}

void CosineNode::ParseNodeLogic(std::stringstream& ss) {

}

bool CosineNode::NodeStateValid() {
	bool pinsAreTypeAny = (InputPins[0].ConnectedPinType == Pin::PinType::ANY);
	return !pinsAreTypeAny;
}

std::string CosineNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}
