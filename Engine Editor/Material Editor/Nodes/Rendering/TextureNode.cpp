#include "TextureNode.h"

TextureNode::TextureNode(IcePick::UUID textureId) {
	m_TextureId = textureId;
	InputPins.emplace_back(Pin::VEC2, "UV");
	OutputPins.emplace_back(Pin::VEC3, "RGBA", ".rgba");
	OutputPins.emplace_back(Pin::FLOAT32, "R", ".r");
	OutputPins.emplace_back(Pin::FLOAT32, "G", ".g");
	OutputPins.emplace_back(Pin::FLOAT32, "B", ".b");
	OutputPins.emplace_back(Pin::FLOAT32, "A", ".a");

	m_NodeType = "texture";
	NodeWidth = 160.0f;
	nodeIsParameter = true;
	NodeName = "Texture";
	NodeHeaderColour = ImU32(0xFF917410);
}

void TextureNode::CustomRendering(IcePick::EngineAPI engineAPI, std::filesystem::path& dropAssetPath, const NodeRenderInfo& renderInfo, ImVec2 canvasScreenPos, ImVec2 canvasScrolling) {
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float nodeMinHeight = renderInfo.PinYSpacing * std::min(InputPins.size(), OutputPins.size()) + 1 + renderInfo.PinRadius + renderInfo.LabelPadding;
	float nodeMaxHeight = renderInfo.PinYSpacing * std::max(InputPins.size(), OutputPins.size());
	ImVec2 imagePos = ImVec2(CanvasPosition.x + canvasScreenPos.x + canvasScrolling.x + renderInfo.PinRadius + renderInfo.LabelPadding, CanvasPosition.y + canvasScreenPos.y + canvasScrolling.y + renderInfo.NodeHeaderHeight + nodeMinHeight);
	ImVec2 imageEnd = ImVec2(0.0f, imagePos.y + nodeMaxHeight - renderInfo.NodePadding);
	imageEnd.x = imagePos.x + imageEnd.y - imagePos.y;
	ImVec2 imageSize = ImVec2(imageEnd.x - imagePos.x, imageEnd.y - imagePos.y);
	draw_list->AddImage((void*)(intptr_t)m_TextureRenderId, imagePos, imageEnd, ImVec2(0, 1), ImVec2(1, 0));

	if (m_TextureRenderId == 0) {
		m_TextureRenderId = engineAPI.GetTextureRenderId(m_TextureId); //Gets default texture for preview
		MaterialInstanceStateChanged = true;
	} 

	ImGui::SetCursorScreenPos(imagePos);
	ImGui::InvisibleButton("textureTarget", imageSize);
	if (ImGui::BeginDragDropTarget()) {
		if (ImGui::AcceptDragDropPayload("TEXTURE_ASSET")) {
			m_TextureId = engineAPI.LoadTextureFromAsset(dropAssetPath);
			m_TextureRenderId = engineAPI.GetTextureRenderId(m_TextureId);
			MaterialInstanceStateChanged = true;
		}
		ImGui::EndDragDropTarget();
	}

}

void TextureNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	std::string sampler = "sampler_" + std::to_string(Id);
	IcePick::MaterialBaseTextureData materialBaseTextureData{ NodeName, sampler, Id };
	editMaterialBase.MaterialTextures.push_back(materialBaseTextureData);

	IcePick::MaterialInstanceData<IcePick::UUID> materialInstanceTextureData{ materialBaseTextureData.Id, m_TextureId };
	editMaterialInstance.InstanceTextureData.push_back(materialInstanceTextureData);

	m_Identifier = "node_" + std::to_string(Id);
	std::string& s1 = InputPins[0].ShaderIdentifier;
	ss << "vec4 " << m_Identifier << " = texture(" << sampler << "," << s1 << ");\n";
	m_Initialised = true;
}

void TextureNode::ParseNodeLogic(std::stringstream& ss) {

}

std::string TextureNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier + OutputPins[outputPinIndex].ShaderAccessor;
}