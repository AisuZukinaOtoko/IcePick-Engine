#pragma once
#include "../NodeBase.h"

class TextureNode : public Node {
public:
	TextureNode(IcePick::UUID textureId);

	void CustomRendering(IcePick::EngineAPI& engineAPI, const NodeRenderInfo& renderInfo, ImVec2 canvasScreenPos, ImVec2 canvasScrolling) override;
	void ParseLogic(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) override;
	std::string GetPinOutput(unsigned int outputPinIndex) override;
	bool NodeStateValid() override;
private:
	IcePick::UUID m_TextureId;
	unsigned int m_TextureRenderId = 0;
};