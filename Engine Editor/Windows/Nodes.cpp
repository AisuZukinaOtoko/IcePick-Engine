#include "Nodes.h"

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

TextureNode::TextureNode(IcePick::UUID textureId) {
	m_TextureId = textureId;
	OutputPins.emplace_back(Pin::VEC3, "RGBA");
	OutputPins.emplace_back(Pin::FLOAT32, "R");
	OutputPins.emplace_back(Pin::FLOAT32, "G");
	OutputPins.emplace_back(Pin::FLOAT32, "B");
	OutputPins.emplace_back(Pin::FLOAT32, "A");

	CanvasPosition.x = 400.0f;
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
	OutputPins.emplace_back(Pin::VEC3, "XYZ");
	OutputPins.emplace_back(Pin::VEC3, "TEMP");
}
