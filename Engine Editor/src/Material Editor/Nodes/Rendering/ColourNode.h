#pragma once
#include "../NodeBase.h"

class ColourNode : public Node {
public:
	ColourNode();

	void CustomRendering(IcePick::EngineAPI& engineAPI, const NodeRenderInfo& renderInfo, ImVec2 canvasScreenPos, ImVec2 canvasScrolling) override;
	void ParseLogic(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) override;
	std::string GetPinOutput(unsigned int outputPinIndex) override;
private:
	ImVec4 m_Value;
};