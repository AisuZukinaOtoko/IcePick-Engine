#include "CrossProductNode.h"
#include "../Utils.h"

CrossProductNode::CrossProductNode() {
	InputPins.emplace_back(Pin::VEC3, "A");
	InputPins.emplace_back(Pin::VEC3, "B");
	OutputPins.emplace_back(Pin::VEC3, "cross(A, B)", "");
	m_NodeType = "crossProduct";
	NodeName = "Cross Product";
	nodeCanBeParamterized = false;
	NodeHeaderColour = ImU32(0xFF1B9F47);
}

void CrossProductNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	m_Identifier = "node_" + std::to_string(Id);
	std::string& sx = InputPins[0].ShaderIdentifier;
	std::string& sy = InputPins[1].ShaderIdentifier;
	ss << "vec3 " << m_Identifier << " = cross(" << sx << ", " << sy << ");\n";
	m_Initialised = true;
}

void CrossProductNode::ParseNodeLogic(std::stringstream& ss) {

}

bool CrossProductNode::NodeStateValid() {
	bool pinsAreTypeVec3 = (InputPins[0].ConnectedPinType == Pin::PinType::VEC3) && (InputPins[1].ConnectedPinType == Pin::PinType::VEC3);
	return pinsAreTypeVec3;
}

std::string CrossProductNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}
