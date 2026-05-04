#include "LengthNode.h"
#include "../Utils.h"

LengthNode::LengthNode() {
	InputPins.emplace_back(Pin::ANY, "A");
	OutputPins.emplace_back(Pin::FLOAT32, "length(A)", "");
	m_NodeType = "length";
	NodeName = "Length";
	nodeCanBeParamterized = false;
	NodeHeaderColour = ImU32(0xFF1B9F47);
}

void LengthNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	m_Identifier = "node_" + std::to_string(Id);
	std::string& sx = InputPins[0].ShaderIdentifier;
	ss << "float " << m_Identifier << " = length(" << sx << ");\n";
	m_Initialised = true;
}

void LengthNode::ParseNodeLogic(std::stringstream& ss) {

}

bool LengthNode::NodeStateValid() {
	bool pinsAreTypeAny = (InputPins[0].ConnectedPinType == Pin::PinType::ANY);
	return !pinsAreTypeAny;
}

std::string LengthNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}
