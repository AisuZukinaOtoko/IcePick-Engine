#include "Nodes.h"
#include "../Utilities/Assert.h"

static std::string pinDefaults[Pin::PIN_TYPE_COUNT];

void Pin::InitPins() {
	pinDefaults[FLOAT32] = "0.0f";
	pinDefaults[VEC3] = "vec3(0.0f)";
	pinDefaults[VEC4] = "vec4(0.0f)";
	pinDefaults[UINT8] = "0";
	pinDefaults[UVEC3] = "uvec3(0)";
	pinDefaults[UVEC4] = "uvec4(0)";
}

std::string Pin::GetPinDefault(PinType type) {
	IP_ASSERT(type != Pin::PIN_TYPE_COUNT, "Invalid pin type.");
	return pinDefaults[type];
}

void InputPin::DeleteConnection() {
	ConnectedNodeId = IcePick::UUID::Unitialised();
	ConnectedPinIndex = 0;
}

void OutputPin::DeleteConnection(IcePick::UUID connectedNode, unsigned int pinIndex) {
	int index = -1;
	for (int i = 0; i < ConnectedNodeIds.size(); i++) {
		if ((ConnectedNodeIds[i] == connectedNode) && (ConnectedPinIndices[i] == pinIndex))
			index = i;
	}

	if (index == -1)
		return;

	ConnectedNodeIds[index] = ConnectedNodeIds.back();
	ConnectedPinIndices[index] = ConnectedPinIndices.back();
	ConnectedNodeIds.pop_back();
	ConnectedPinIndices.pop_back();
}

void Node::Unitialise() {
	m_Initialised = false;
	m_Identifier = "";
}

TextureNode::TextureNode(IcePick::UUID textureId) {
	m_TextureId = textureId;
	OutputPins.emplace_back(Pin::VEC3, "RGBA", ".rgba");
	OutputPins.emplace_back(Pin::FLOAT32, "R", ".r");
	OutputPins.emplace_back(Pin::FLOAT32, "G", ".g");
	OutputPins.emplace_back(Pin::FLOAT32, "B", ".b");
	OutputPins.emplace_back(Pin::FLOAT32, "A", ".a");

	CanvasPosition.x = 400.0f;
}

void TextureNode::Initialise(std::stringstream& ss) {
	if (m_Initialised)
		return;

	m_Identifier = "node_" + std::to_string(Id);
	ss << "vec4 " << m_Identifier << " = texture(" << m_TextureId << ");\n";
	m_Initialised = true;
}

void TextureNode::ParseNodeLogic(std::stringstream& ss) {
	
}

std::string TextureNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier + OutputPins[outputPinIndex].ShaderAccessor;
}

BSDFNode::BSDFNode() {
	InputPins.emplace_back(Pin::VEC4, "Albedo");
	//InputPins.emplace_back(Pin::VEC3, "Normal");
	//InputPins.emplace_back(Pin::FLOAT32, "Roughness");
	//InputPins.emplace_back(Pin::FLOAT32, "Metallic");
	//InputPins.emplace_back(Pin::FLOAT32, "Emissive");
}

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

