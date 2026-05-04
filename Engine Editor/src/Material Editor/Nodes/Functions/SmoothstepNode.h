#pragma once
#include "../NodeBase.h"

class SmoothstepNode : public Node {
public:
	SmoothstepNode();

	void Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) override;
	void ParseNodeLogic(std::stringstream& ss) override;
	std::string GetPinOutput(unsigned int outputPinIndex) override;
	bool NodeStateValid() override;
};