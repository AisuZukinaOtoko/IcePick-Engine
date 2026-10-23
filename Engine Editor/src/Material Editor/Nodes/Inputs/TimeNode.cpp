#include "TimeNode.h"

TimeNode::TimeNode() {
	OutputPins.emplace_back(Pin::FLOAT32, "ms", "");
	NodeName = "Time";
	m_NodeType = "time";
	NodeHeaderColour = ImU32(0xFF001CC8);
}

void TimeNode::ParseLogic(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	m_NodeVisited = true;
}

std::string TimeNode::GetPinOutput(unsigned int outputPinIndex) {
	return "AccumulatedTime";
}
