#pragma once
#include "../NodeBase.h"

class LengthNode : public Node {
public:
	LengthNode();

	void ParseLogic(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) override;
	std::string GetPinOutput(unsigned int outputPinIndex) override;
	bool NodeStateValid() override;
};