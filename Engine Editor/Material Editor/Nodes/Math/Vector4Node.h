#pragma once
#include "../NodeBase.h"

class Vector4Node : public Node {
public:
	Vector4Node();

	void Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) override;
	void ParseNodeLogic(std::stringstream& ss) override;
	std::string GetPinOutput(unsigned int outputPinIndex) override;
};