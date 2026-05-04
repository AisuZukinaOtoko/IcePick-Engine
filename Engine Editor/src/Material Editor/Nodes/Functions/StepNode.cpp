#include "StepNode.h"
#include "../Utils.h"

StepNode::StepNode() {
	InputPins.emplace_back(Pin::ANY, "Value");
	InputPins.emplace_back(Pin::ANY, "Edge");
	OutputPins.emplace_back(Pin::ANY, "Out", "");
	m_NodeType = "step";
	NodeName = "Step";
	nodeCanBeParamterized = false;
	NodeHeaderColour = ImU32(0xFF1B9F47);
}

void StepNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	OutputPins[0].Type = InputPins[0].ConnectedPinType;
	m_Identifier = "node_" + std::to_string(Id);
	std::string& sx = InputPins[0].ShaderIdentifier;
	std::string& sy = InputPins[1].ShaderIdentifier;
	std::string type = GetPinTypeString(InputPins[0].ConnectedPinType);
	ss << type << " " << m_Identifier << " = step(" << sy << ", " << sx << ");\n";
	m_Initialised = true;
}

void StepNode::ParseNodeLogic(std::stringstream& ss) {

}

bool StepNode::NodeStateValid() {
	bool pinsAreTypeAny = (InputPins[0].ConnectedPinType == Pin::PinType::ANY) || (InputPins[1].ConnectedPinType == Pin::PinType::ANY);
	bool pinTypesMatch = (InputPins[0].ConnectedPinType == InputPins[1].ConnectedPinType);
	bool edgePinIsScalar = (InputPins[1].ConnectedPinType == Pin::PinType::FLOAT32);
	return (!pinsAreTypeAny) && (edgePinIsScalar || pinTypesMatch);
}

std::string StepNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}
