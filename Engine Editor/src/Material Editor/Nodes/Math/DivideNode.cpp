#include "DivideNode.h"
#include "../Utils.h"

DivideNode::DivideNode() {
	InputPins.emplace_back(Pin::ANY, "A");
	InputPins.emplace_back(Pin::ANY, "B");
	OutputPins.emplace_back(Pin::ANY, "A / B", "");
	m_NodeType = "divide";
	NodeName = "Divide";
	nodeCanBeParamterized = false;
}

void DivideNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	OutputPins[0].Type = InputPins[0].ConnectedPinType;
	m_Identifier = "node_" + std::to_string(Id);
	std::string& sx = InputPins[0].ShaderIdentifier;
	std::string& sy = InputPins[1].ShaderIdentifier;
	std::string type = GetPinTypeString(InputPins[0].ConnectedPinType);
	ss << type << " " << m_Identifier << " = " << sx << " / " << sy << ";\n";
	m_Initialised = true;
}

void DivideNode::ParseNodeLogic(std::stringstream& ss) {

}

bool DivideNode::NodeStateValid() {
	bool pinsAreTypeAny = (InputPins[0].ConnectedPinType == Pin::PinType::ANY) || (InputPins[1].ConnectedPinType == Pin::PinType::ANY);
	bool pinTypesMatch = (InputPins[0].ConnectedPinType == InputPins[1].ConnectedPinType);
	bool denominatorIsScalar = (InputPins[1].ConnectedPinType == Pin::PinType::FLOAT32);
	return !pinsAreTypeAny && (denominatorIsScalar || pinTypesMatch);
}

std::string DivideNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}
