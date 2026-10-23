#pragma once
#include "../NodeBase.h"

class FragmentCoordinateNode : public Node {
public:
	FragmentCoordinateNode();

	void ParseLogic(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) override;
	std::string GetPinOutput(unsigned int outputPinIndex) override;
};