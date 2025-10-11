#include "Vector3Node.h"

Vector3Node::Vector3Node() {
	InputPins.emplace_back(Pin::FLOAT32, "X");
	InputPins.emplace_back(Pin::FLOAT32, "Y");
	InputPins.emplace_back(Pin::FLOAT32, "Z");
	OutputPins.emplace_back(Pin::VEC3, "XYZ", ".xyz");
}

void Vector3Node::ParseNodeLogic(std::stringstream& ss) {
	std::string& s1 = InputPins[0].ShaderIdentifier;
	std::string& s2 = InputPins[1].ShaderIdentifier;
	std::string& s3 = InputPins[2].ShaderIdentifier;
	ss << "vec3 node_" << std::to_string(Id) << " = vec3(" << s1 << ", " << s2 << ", " << s3 << "); \n";
}