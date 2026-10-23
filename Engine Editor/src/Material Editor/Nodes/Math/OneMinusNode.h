#pragma once
#include "../NodeBase.h"

class OneMinusNode : public Node {
public:
	OneMinusNode();

	void ParseLogic(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) override;
	bool NodeStateValid() override;
	std::string GetPinOutput(unsigned int outputPinIndex) override;
};