#include "TimeNode.h"

TimeNode::TimeNode() {
	OutputPins.emplace_back(Pin::FLOAT32, "ms", "");
	NodeName = "Time";
	m_NodeType = "time";
	NodeHeaderColour = ImU32(0xFF001CC8);
}

void TimeNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {

}

void TimeNode::ParseNodeLogic(std::stringstream& ss) {

}

std::string TimeNode::GetPinOutput(unsigned int outputPinIndex) {
	return "AccumulatedTime";
}
