#pragma once
#include "../NodeBase.h"

class CameraViewNode : public Node {
public:
	CameraViewNode();

	void ParseLogic(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) override;
	std::string GetPinOutput(unsigned int outputPinIndex) override;
};