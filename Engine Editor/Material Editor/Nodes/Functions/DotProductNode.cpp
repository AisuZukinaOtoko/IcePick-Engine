#include "DotProductNode.h"
#include "../Utils.h"

DotProductNode::DotProductNode() {
	InputPins.emplace_back(Pin::ANY, "A");
	InputPins.emplace_back(Pin::ANY, "B");
	OutputPins.emplace_back(Pin::FLOAT32, "dot(A, B)", "");
	m_NodeType = "dotProduct";
	NodeName = "Dot Product";
	nodeCanBeParamterized = false;
	NodeHeaderColour = ImU32(0xFF1B9F47);
}

void DotProductNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	m_Identifier = "node_" + std::to_string(Id);
	std::string& sx = InputPins[0].ShaderIdentifier;
	std::string& sy = InputPins[1].ShaderIdentifier;
	ss << "float " << m_Identifier << " = dot(" << sx << ", " << sy << ");\n";
	m_Initialised = true;
}

void DotProductNode::ParseNodeLogic(std::stringstream& ss) {

}

bool DotProductNode::NodeStateValid() {
	bool pinsAreTypeAny = (InputPins[0].ConnectedPinType == Pin::PinType::ANY) || (InputPins[1].ConnectedPinType == Pin::PinType::ANY);
	bool pinTypesMatch = (InputPins[0].ConnectedPinType == InputPins[1].ConnectedPinType);
	return !pinsAreTypeAny && pinTypesMatch;
}

std::string DotProductNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}
