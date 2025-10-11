#pragma once
#include "../NodeBase.h"

class TextureNode : public Node {
public:
	TextureNode(IcePick::UUID textureId);

	void CustomRendering(IcePick::EngineAPI engineAPI, const NodeRenderInfo& renderInfo, ImVec2 canvasScreenPos, ImVec2 canvasScrolling) override;
	void Initialise(std::stringstream& ss) override;
	void ParseNodeLogic(std::stringstream& ss) override;
	std::string GetPinOutput(unsigned int outputPinIndex) override;
private:
	IcePick::UUID m_TextureId;
};