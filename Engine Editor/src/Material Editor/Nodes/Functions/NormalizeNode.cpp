#include "NormalizeNode.h"
#include "../Utils.h"

NormalizeNode::NormalizeNode() {
	InputPins.emplace_back(Pin::ANY, "A");
	OutputPins.emplace_back(Pin::ANY, "normalize(A)", "");
	m_NodeType = "normalize";
	NodeName = "Normalize";
	nodeCanBeParamterized = false;
	NodeHeaderColour = ImU32(0xFF1B9F47);
}

void NormalizeNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	OutputPins[0].Type = InputPins[0].ConnectedPinType;
	m_Identifier = "node_" + std::to_string(Id);
	std::string& sx = InputPins[0].ShaderIdentifier;
	std::string type = GetPinTypeString(InputPins[0].ConnectedPinType);
	ss << type << " " << m_Identifier << " = normalize(" << sx << ");\n";
	m_Initialised = true;
}

void NormalizeNode::ParseNodeLogic(std::stringstream& ss) {

}

bool NormalizeNode::NodeStateValid() {
	bool pinsAreTypeAny = (InputPins[0].ConnectedPinType == Pin::PinType::ANY);
	return !pinsAreTypeAny;
}

std::string NormalizeNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}
