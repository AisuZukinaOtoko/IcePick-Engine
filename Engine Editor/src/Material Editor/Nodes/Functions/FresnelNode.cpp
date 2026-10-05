#include "FresnelNode.h"

FresnelNode::FresnelNode() {
	InputPins.emplace_back(Pin::VEC3, "Normal");
	InputPins.emplace_back(Pin::VEC3, "View");
	InputPins.emplace_back(Pin::FLOAT32, "Power");
	OutputPins.emplace_back(Pin::FLOAT32, "Out", "");
	m_NodeType = "fresnel";
	NodeName = "Fresnel";
	nodeCanBeParamterized = false;
	NodeHeaderColour = ImU32(0xFF1B9F47);
	m_Identifier = "node_" + std::to_string(Id);
}

void FresnelNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	std::string& normal = InputPins[0].ShaderIdentifier;
	std::string& view = InputPins[1].ShaderIdentifier;
	std::string& power = InputPins[2].ShaderIdentifier;
	ss <<
		"float " << m_Identifier <<
		" = pow(" << 
		"1.0f - max(dot(normalize(" << normal << "), normalize(" << view << ")), 0.0f)" <<
		", " << power << ");\n";
	m_Initialised = true;
}

void FresnelNode::ParseNodeLogic(std::stringstream& ss) {

}

bool FresnelNode::NodeStateValid() {
	bool vectorsAre3Component = (InputPins[0].ConnectedPinType == Pin::PinType::VEC3) && (InputPins[1].ConnectedPinType == Pin::PinType::VEC3);
	bool powerIsFloat = (InputPins[2].ConnectedPinType == Pin::PinType::FLOAT32);
	return vectorsAre3Component && powerIsFloat;
}

std::string FresnelNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}
