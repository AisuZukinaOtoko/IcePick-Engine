#pragma once
#include "../NodeBase.h"

class Vector4Node : public Node {
public:
	Vector4Node();

	void Initialise(std::stringstream& ss, IcePick::MaterialAsset& editMaterial) override;
	void ParseNodeLogic(std::stringstream& ss) override;
	std::string GetPinOutput(unsigned int outputPinIndex) override;
};