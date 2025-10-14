#include "Vector4Node.h"

Vector4Node::Vector4Node() {
	InputPins.emplace_back(Pin::FLOAT32, "X");
	InputPins.emplace_back(Pin::FLOAT32, "Y");
	InputPins.emplace_back(Pin::FLOAT32, "Z");
	InputPins.emplace_back(Pin::FLOAT32, "W");
	OutputPins.emplace_back(Pin::VEC3, "XYZW", ".xyzw");
}

void Vector4Node::Initialise(std::stringstream& ss, IcePick::MaterialAsset& editMaterial) {
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
	//std::string& s1 = InputPins[0].ShaderIdentifier;
	//std::string& s2 = InputPins[1].ShaderIdentifier;
	//std::string& s3 = InputPins[2].ShaderIdentifier;
	//ss << "vec3 node_" << std::to_string(Id) << " = vec3(" << s1 << ", " << s2 << ", " << s3 << "); \n";
}

std::string Vector4Node::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}