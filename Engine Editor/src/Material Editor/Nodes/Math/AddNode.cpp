#include "AddNode.h"
#include "../Utils.h"

AddNode::AddNode() {
	InputPins.emplace_back(Pin::ANY, "A");
	InputPins.emplace_back(Pin::ANY, "B");
	OutputPins.emplace_back(Pin::ANY, "A + B", "");
	m_NodeType = "add";
	NodeName = "Add";
	m_Identifier = "node_" + std::to_string(Id);
}

void AddNode::ParseLogic(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_NodeVisited)
		return;

	OutputPins[0].Type = InputPins[0].ConnectedPinType;
	std::string& sx = InputPins[0].ShaderIdentifier;
	std::string& sy = InputPins[1].ShaderIdentifier;
	std::string type = GetPinTypeString(InputPins[0].ConnectedPinType);
	ss << type << " " << m_Identifier << " = " << sx << " + " << sy << ";\n";
	m_NodeVisited = true;
}

bool AddNode::NodeStateValid() {
	bool pinsAreTypeAny = (InputPins[0].ConnectedPinType == Pin::PinType::ANY) || (InputPins[1].ConnectedPinType == Pin::PinType::ANY);
	return (InputPins[0].ConnectedPinType == InputPins[1].ConnectedPinType) && !pinsAreTypeAny;
}

std::string AddNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}
