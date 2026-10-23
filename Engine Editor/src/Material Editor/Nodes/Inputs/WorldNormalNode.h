#pragma once
#include "../NodeBase.h"

class WorldNormalNode : public Node {
public:
	WorldNormalNode();

	void ParseLogic(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) override;
	std::string GetPinOutput(unsigned int outputPinIndex) override;
};