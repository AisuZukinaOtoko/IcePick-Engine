#include "UVNode.h"

UVNode::UVNode() {
	OutputPins.emplace_back(Pin::VEC2, "UV", "");
	NodeName = "Model UV";
	m_NodeType = "uv";
	NodeHeaderColour = ImU32(0xFF001CC8);
}

void UVNode::ParseLogic(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	m_NodeVisited = true;
}

std::string UVNode::GetPinOutput(unsigned int outputPinIndex) {
	return "v_TexCoord";
}
