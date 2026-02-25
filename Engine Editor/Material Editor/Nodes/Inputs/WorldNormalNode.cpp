#include "WorldNormalNode.h"

WorldNormalNode::WorldNormalNode() {
	OutputPins.emplace_back(Pin::VEC3, "World Normal", "");
	NodeName = "World Normal";
	m_NodeType = "worldNormal";
	NodeHeaderColour = ImU32(0xFF001CC8);
}

void WorldNormalNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {

}

void WorldNormalNode::ParseNodeLogic(std::stringstream& ss) {

}

std::string WorldNormalNode::GetPinOutput(unsigned int outputPinIndex) {
	return "v_Normal";
}
