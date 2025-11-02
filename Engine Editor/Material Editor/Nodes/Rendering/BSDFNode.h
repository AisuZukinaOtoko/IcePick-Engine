#pragma once
#include "../NodeBase.h"

class BSDFNode : public Node {
public:
	BSDFNode();
	virtual void Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) override;
	virtual void ParseNodeLogic(std::stringstream& ss) override;
	virtual std::string GetPinOutput(unsigned int outputPinIndex) override;
};