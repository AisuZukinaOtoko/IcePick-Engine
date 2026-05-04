#include "WorldPositionNode.h"
#include "../Utils.h"

WorldPositionNode::WorldPositionNode() {
	OutputPins.emplace_back(Pin::VEC3, "World Position", "");
	m_NodeType = "worldPosition";
	NodeName = "World Position";
	nodeCanBeParamterized = false;
	NodeHeaderColour = ImU32(0xFF001CC8);
}

void WorldPositionNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {

}

void WorldPositionNode::ParseNodeLogic(std::stringstream& ss) {

}

std::string WorldPositionNode::GetPinOutput(unsigned int outputPinIndex) {
	return "v_Pos";
}
