#pragma once
#include "../NodeBase.h"

class UVNode : public Node {
public:
	UVNode();

	void Initialise(std::stringstream& ss, IcePick::MaterialAsset& editMaterial) override;
	void ParseNodeLogic(std::stringstream& ss) override;
	std::string GetPinOutput(unsigned int outputPinIndex) override;
};