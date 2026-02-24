#pragma once
#include "../NodeBase.h"

class SubtractNode : public Node {
public:
	SubtractNode();

	void Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) override;
	void ParseNodeLogic(std::stringstream& ss) override;
	std::string GetPinOutput(unsigned int outputPinIndex) override;
	bool NodeStateValid() override;
};