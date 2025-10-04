#include "Nodes.h"

TextureNode::TextureNode(IcePick::UUID textureId) {
	m_TextureId = textureId;
	OutputPins.emplace_back(Pin::FLOAT32, "R");
	OutputPins.emplace_back(Pin::FLOAT32, "G");
	OutputPins.emplace_back(Pin::FLOAT32, "B");
	OutputPins.emplace_back(Pin::FLOAT32, "A");
	OutputPins.emplace_back(Pin::VEC3, "RGB");

	CanvasPosition.x = 400.0f;
}

BSDFNode::BSDFNode() {
	InputPins.emplace_back(Pin::VEC4, "Albedo");
	InputPins.emplace_back(Pin::VEC3, "Normal");
	InputPins.emplace_back(Pin::FLOAT32, "Roughness");
	InputPins.emplace_back(Pin::FLOAT32, "Metallic");
	InputPins.emplace_back(Pin::FLOAT32, "Emissive");
}
