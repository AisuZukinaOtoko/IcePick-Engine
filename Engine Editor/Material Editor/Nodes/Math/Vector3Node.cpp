#include "Vector3Node.h"

Vector3Node::Vector3Node() {
	InputPins.emplace_back(Pin::FLOAT32, "X");
	InputPins.emplace_back(Pin::FLOAT32, "Y");
	InputPins.emplace_back(Pin::FLOAT32, "Z");
	OutputPins.emplace_back(Pin::VEC3, "XYZ", ".xyz");
	m_NodeType = "vec3";
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
	//std::string& s1 = InputPins[0].ShaderIdentifier;
	//std::string& s2 = InputPins[1].ShaderIdentifier;
	//std::string& s3 = InputPins[2].ShaderIdentifier;
	//ss << "vec3 node_" << std::to_string(Id) << " = vec3(" << s1 << ", " << s2 << ", " << s3 << "); \n";
}

std::string Vector3Node::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}
