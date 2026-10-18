#pragma once
#include "../NodeBase.h"

class OneMinusNode : public Node {
public:
	OneMinusNode();

	void Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) override;
	void ParseNodeLogic(std::stringstream& ss) override;
	bool NodeStateValid() override;
	std::string GetPinOutput(unsigned int outputPinIndex) override;
};