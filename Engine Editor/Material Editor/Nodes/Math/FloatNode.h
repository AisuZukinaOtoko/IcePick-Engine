#pragma once
#include "../NodeBase.h"

class FloatNode : public Node {
public:
	FloatNode();

	void CustomRendering(IcePick::EngineAPI engineAPI, std::filesystem::path& dropAssetPath, const NodeRenderInfo& renderInfo, ImVec2 canvasScreenPos, ImVec2 canvasScrolling) override;
	void Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) override;
	void ParseNodeLogic(std::stringstream& ss) override;
	std::string GetPinOutput(unsigned int outputPinIndex) override;
private:
	float m_Value = 0.0f;
};