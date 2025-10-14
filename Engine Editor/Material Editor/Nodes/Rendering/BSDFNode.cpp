#include "BSDFNode.h"

BSDFNode::BSDFNode() {
	InputPins.emplace_back(Pin::VEC4, "Albedo");
	//InputPins.emplace_back(Pin::VEC3, "Normal");
	//InputPins.emplace_back(Pin::FLOAT32, "Roughness");
	//InputPins.emplace_back(Pin::FLOAT32, "Metallic");
	//InputPins.emplace_back(Pin::FLOAT32, "Emissive");
}

void BSDFNode::Initialise(std::stringstream& ss, IcePick::MaterialAsset& editMaterial) {
	if (m_Initialised)
		return;

	m_Identifier = "node_" + std::to_string(Id);
	std::string& sAlbedo = InputPins[0].ShaderIdentifier;
	ss << "OutColour = " << sAlbedo << ";\n";
	m_Initialised = true;
}

void BSDFNode::ParseNodeLogic(std::stringstream& ss) {

}

std::string BSDFNode::GetPinOutput(unsigned int outputPinIndex) {
	return "";
}
