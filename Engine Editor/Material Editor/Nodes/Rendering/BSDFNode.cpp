#include "BSDFNode.h"

BSDFNode::BSDFNode() {
	InputPins.emplace_back(Pin::VEC4, "Albedo");
	//InputPins.emplace_back(Pin::VEC3, "Normal");
	//InputPins.emplace_back(Pin::FLOAT32, "Roughness");
	//InputPins.emplace_back(Pin::FLOAT32, "Metallic");
	//InputPins.emplace_back(Pin::FLOAT32, "Emissive");
	NodeName = "Output";
	m_NodeType = "bsdf";
}

void BSDFNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	m_Identifier = "node_" + std::to_string(Id);
	std::string& sAlbedo = InputPins[0].ShaderIdentifier;
	ss << "OutColour = " << sAlbedo << ";\n";
	m_Initialised = true;
}

void BSDFNode::ParseNodeLogic(std::stringstream& ss) {

}

bool BSDFNode::NodeStateValid() {
	bool pinIsTypeVec4 = (InputPins[0].ConnectedPinType == Pin::PinType::VEC4);
	return pinIsTypeVec4;
}

std::string BSDFNode::GetPinOutput(unsigned int outputPinIndex) {
	return "";
}
