#include "UVNode.h"

UVNode::UVNode() {
	OutputPins.emplace_back(Pin::VEC2, "UV", "");
}

void UVNode::Initialise(std::stringstream& ss, IcePick::MaterialAsset& editMaterial) {

}

void UVNode::ParseNodeLogic(std::stringstream& ss) {

}

std::string UVNode::GetPinOutput(unsigned int outputPinIndex) {
	return "v_TexCoord";
}
