#pragma once
#include "../NodeBase.h"

class VoronoiNoiseNode : public Node {
public:
	VoronoiNoiseNode();

	void ParseLogic(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) override;
	std::string GetPinOutput(unsigned int outputPinIndex) override;
	bool NodeStateValid() override;
};