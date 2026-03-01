#include "VoronoiNoiseNode.h"
#include "../Utils.h"

VoronoiNoiseNode::VoronoiNoiseNode() {
	InputPins.emplace_back(Pin::VEC2, "UV");
	InputPins.emplace_back(Pin::FLOAT32, "T");
	OutputPins.emplace_back(Pin::FLOAT32, "Out", "");
	m_NodeType = "voronoiNoise";
	NodeName = "Voronoi Noise";
	nodeCanBeParamterized = false;
	NodeHeaderColour = ImU32(0xFF917410);
}

void VoronoiNoiseNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	m_Identifier = "node_" + std::to_string(Id);
	std::string& sUV = InputPins[0].ShaderIdentifier;
	std::string& st = InputPins[1].ShaderIdentifier;
	ss << "float " << m_Identifier << " = VoronoiNoise(" << sUV << ", " << st << ");\n";
	m_Initialised = true;
}

void VoronoiNoiseNode::ParseNodeLogic(std::stringstream& ss) {

}

bool VoronoiNoiseNode::NodeStateValid() {
	bool pinsAreValid = (InputPins[0].ConnectedPinType == Pin::PinType::VEC2) && (InputPins[1].ConnectedPinType == Pin::PinType::FLOAT32);
	return pinsAreValid;
}

std::string VoronoiNoiseNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}
