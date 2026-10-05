#pragma once
#include "../NodeBase.h"

class ColourNode : public Node {
public:
	ColourNode();

	void CustomRendering(IcePick::EngineAPI& engineAPI, std::filesystem::path& dropAssetPath, const NodeRenderInfo& renderInfo, ImVec2 canvasScreenPos, ImVec2 canvasScrolling) override;
	void Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) override;
	void ParseNodeLogic(std::stringstream& ss) override;
	std::string GetPinOutput(unsigned int outputPinIndex) override;
private:
	ImVec4 m_Value;
};