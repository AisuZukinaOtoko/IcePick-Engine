#pragma once
#include "../NodeBase.h"

class FresnelNode : public Node {
public:
	FresnelNode();

	void Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) override;
	void ParseNodeLogic(std::stringstream& ss) override;
	std::string GetPinOutput(unsigned int outputPinIndex) override;
	bool NodeStateValid() override;
};