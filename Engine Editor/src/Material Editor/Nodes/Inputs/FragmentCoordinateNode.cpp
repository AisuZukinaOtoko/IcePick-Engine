#include "FragmentCoordinateNode.h"

FragmentCoordinateNode::FragmentCoordinateNode() {
	OutputPins.emplace_back(Pin::VEC4, "Fragment Coordinate", "");
	NodeName = "Frag Coord";
	m_NodeType = "fragmentCoordinate";
	NodeHeaderColour = ImU32(0xFF001CC8);
}

void FragmentCoordinateNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {

}

void FragmentCoordinateNode::ParseNodeLogic(std::stringstream& ss) {

}

std::string FragmentCoordinateNode::GetPinOutput(unsigned int outputPinIndex) {
	return "gl_FragCoord";
}
