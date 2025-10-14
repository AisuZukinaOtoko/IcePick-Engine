#pragma once
#include "../NodeBase.h"

class BSDFNode : public Node {
public:
	BSDFNode();
	virtual void Initialise(std::stringstream& ss, IcePick::MaterialAsset& editMaterial) override;
	virtual void ParseNodeLogic(std::stringstream& ss) override;
	virtual std::string GetPinOutput(unsigned int outputPinIndex) override;
};