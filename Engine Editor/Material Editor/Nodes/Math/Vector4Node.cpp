#include "Vector4Node.h"

Vector4Node::Vector4Node() {
	InputPins.emplace_back(Pin::FLOAT32, "X");
	InputPins.emplace_back(Pin::FLOAT32, "Y");
	InputPins.emplace_back(Pin::FLOAT32, "Z");
	InputPins.emplace_back(Pin::FLOAT32, "W");
	OutputPins.emplace_back(Pin::VEC4, "XYZW", ".xyzw");
	m_NodeType = "vec4";
	NodeName = "Vector4";
	nodeCanBeParamterized = true;
}

void Vector4Node::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	m_Identifier = "node_" + std::to_string(Id);
	std::string& sx = InputPins[0].ShaderIdentifier;
	std::string& sy = InputPins[1].ShaderIdentifier;
	std::string& sz = InputPins[2].ShaderIdentifier;
	std::string& sw = InputPins[3].ShaderIdentifier;
	ss << "vec4 " << m_Identifier << " = vec4(" << sx << "," << sy << "," << sz << "," << sw << ");\n";
	m_Initialised = true;
}

void Vector4Node::ParseNodeLogic(std::stringstream& ss) {
	
}

bool Vector4Node::NodeStateValid() {
	bool pin1IsScalarOrDefault = (InputPins[0].ConnectedPinType == Pin::PinType::FLOAT32) || (InputPins[0].ConnectedPinType == Pin::PinType::ANY);
	bool pin2IsScalarOrDefault = (InputPins[1].ConnectedPinType == Pin::PinType::FLOAT32) || (InputPins[1].ConnectedPinType == Pin::PinType::ANY);
	bool pin3IsScalarOrDefault = (InputPins[2].ConnectedPinType == Pin::PinType::FLOAT32) || (InputPins[2].ConnectedPinType == Pin::PinType::ANY);
	bool pin4IsScalarOrDefault = (InputPins[3].ConnectedPinType == Pin::PinType::FLOAT32) || (InputPins[3].ConnectedPinType == Pin::PinType::ANY);
	
	return pin1IsScalarOrDefault && pin2IsScalarOrDefault && pin3IsScalarOrDefault && pin4IsScalarOrDefault;
}

std::string Vector4Node::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}