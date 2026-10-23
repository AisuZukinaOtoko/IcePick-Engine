#pragma once
#include "../NodeBase.h"

class BSDFNode : public Node {
public:
	BSDFNode();
	virtual void InitialiseNodeState(IcePick::MaterialBase& editMaterialBase) override;
	virtual void ParseLogic(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) override;
	virtual std::string GetPinOutput(unsigned int outputPinIndex) override;
	bool NodeStateValid() override;
};