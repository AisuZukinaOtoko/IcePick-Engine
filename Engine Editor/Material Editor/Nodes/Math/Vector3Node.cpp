#include "Vector3Node.h"

Vector3Node::Vector3Node() {
	InputPins.emplace_back(Pin::FLOAT32, "X");
	InputPins.emplace_back(Pin::FLOAT32, "Y");
	InputPins.emplace_back(Pin::FLOAT32, "Z");
	OutputPins.emplace_back(Pin::VEC3, "XYZ", ".xyz");
	m_NodeType = "vec3";
	NodeName = "Vector3";
	nodeCanBeParamterized = false;
}

void Vector3Node::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	m_Identifier = "node_" + std::to_string(Id);
	std::string& sx = InputPins[0].ShaderIdentifier;
	std::string& sy = InputPins[1].ShaderIdentifier;
	std::string& sz = InputPins[2].ShaderIdentifier;
	ss << "vec3 " << m_Identifier << " = vec3(" << sx << "," << sy << "," << sz << ");\n";
	m_Initialised = true;
}

void Vector3Node::ParseNodeLogic(std::stringstream& ss) {

}

bool Vector3Node::NodeStateValid() {
	bool pin1IsScalarOrDefault = (InputPins[0].ConnectedPinType == Pin::PinType::FLOAT32) || (InputPins[0].ConnectedNodeId == IcePick::UUID::Unitialised());
	bool pin2IsScalarOrDefault = (InputPins[1].ConnectedPinType == Pin::PinType::FLOAT32) || (InputPins[1].ConnectedNodeId == IcePick::UUID::Unitialised());
	bool pin3IsScalarOrDefault = (InputPins[2].ConnectedPinType == Pin::PinType::FLOAT32) || (InputPins[2].ConnectedNodeId == IcePick::UUID::Unitialised());

	return pin1IsScalarOrDefault && pin2IsScalarOrDefault && pin3IsScalarOrDefault;
}

std::string Vector3Node::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}
