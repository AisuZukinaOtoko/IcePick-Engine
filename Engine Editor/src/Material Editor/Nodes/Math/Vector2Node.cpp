#include "Vector2Node.h"

Vector2Node::Vector2Node() {
	InputPins.emplace_back(Pin::FLOAT32, "X");
	InputPins.emplace_back(Pin::FLOAT32, "Y");
	OutputPins.emplace_back(Pin::VEC2, "XY", ".xy");
	m_NodeType = "vec2";
	NodeName = "Vector2";
	nodeCanBeParamterized = false;
}

void Vector2Node::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	m_Identifier = "node_" + std::to_string(Id);
	std::string& sx = InputPins[0].ShaderIdentifier;
	std::string& sy = InputPins[1].ShaderIdentifier;
	ss << "vec2 " << m_Identifier << " = vec2(" << sx << "," << sy << ");\n";
	m_Initialised = true;
}

void Vector2Node::ParseNodeLogic(std::stringstream& ss) {
	
}

bool Vector2Node::NodeStateValid() {
	bool pin1IsScalarOrDefault = (InputPins[0].ConnectedPinType == Pin::PinType::FLOAT32) || (InputPins[0].ConnectedNodeId == IcePick::UUID::Unitialised());
	bool pin2IsScalarOrDefault = (InputPins[1].ConnectedPinType == Pin::PinType::FLOAT32) || (InputPins[1].ConnectedNodeId == IcePick::UUID::Unitialised());

	return pin1IsScalarOrDefault && pin2IsScalarOrDefault;
}

std::string Vector2Node::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}
