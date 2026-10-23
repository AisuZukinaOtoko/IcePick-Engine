#pragma once
#include "../NodeBase.h"

class WorldPositionNode : public Node {
public:
	WorldPositionNode();

	void ParseLogic(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) override;
	std::string GetPinOutput(unsigned int outputPinIndex) override;
};