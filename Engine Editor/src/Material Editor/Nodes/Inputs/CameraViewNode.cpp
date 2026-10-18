#include "CameraViewNode.h"

CameraViewNode::CameraViewNode() {
	OutputPins.emplace_back(Pin::VEC3, "View", "");
	NodeName = "Camera View";
	m_NodeType = "cameraView";
	NodeHeaderColour = ImU32(0xFF001CC8);
	m_Identifier = "node_" + std::to_string(Id);
}

void CameraViewNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	ss << "vec3 " << m_Identifier << " = CameraPosition - v_Pos;\n";
	m_Initialised = true;
}

void CameraViewNode::ParseNodeLogic(std::stringstream& ss) {

}

std::string CameraViewNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier;
}
