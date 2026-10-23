#pragma once
#include "../NodeBase.h"

class FloatNode : public Node {
public:
	FloatNode();

	void CustomRendering(IcePick::EngineAPI& engineAPI, const NodeRenderInfo& renderInfo, ImVec2 canvasScreenPos, ImVec2 canvasScrolling) override;
	void ParseLogic(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) override;
	std::string GetPinOutput(unsigned int outputPinIndex) override;
private:
	float m_Value = 0.0f;
};