#include "TextureNode.h"

TextureNode::TextureNode(IcePick::UUID textureId) {
	m_TextureId = textureId;
	InputPins.emplace_back(Pin::VEC2, "UV");
	OutputPins.emplace_back(Pin::VEC3, "RGBA", ".rgba");
	OutputPins.emplace_back(Pin::FLOAT32, "R", ".r");
	OutputPins.emplace_back(Pin::FLOAT32, "G", ".g");
	OutputPins.emplace_back(Pin::FLOAT32, "B", ".b");
	OutputPins.emplace_back(Pin::FLOAT32, "A", ".a");

	NodeWidth = 200.0f;
}

void TextureNode::CustomRendering(IcePick::EngineAPI engineAPI, const NodeRenderInfo& renderInfo, ImVec2 canvasScreenPos, ImVec2 canvasScrolling) {
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	unsigned int textureId = engineAPI.GetTextureRenderId(m_TextureId);
	ImVec2 imagePos = ImVec2(CanvasPosition.x + canvasScreenPos.x + canvasScrolling.x, CanvasPosition.y + canvasScreenPos.y + canvasScrolling.y + renderInfo.NodeHeaderHeight);
	ImVec2 imageSize = ImVec2(100.0f, 100.0f);
	ImVec2 imageEnd = ImVec2(imagePos.x + imageSize.x, imagePos.y + imageSize.y);
	draw_list->AddImage((void*)(intptr_t)textureId, imagePos, imageEnd, ImVec2(0, 1), ImVec2(1, 0));

	ImGui::SetCursorScreenPos(imagePos);
	ImGui::InvisibleButton("textureTarget", imageSize);
	if (ImGui::BeginDragDropTarget()) {
		if (ImGui::AcceptDragDropPayload("ASSET")) {
			//DropMaterialIntoViewport();
			m_TextureId = engineAPI.LoadTextureFromAsset("res/textures/black_fabric_diffuse.iptex");
		}
		ImGui::EndDragDropTarget();
	}

}

void TextureNode::Initialise(std::stringstream& ss) {
	if (m_Initialised)
		return;

	m_Identifier = "node_" + std::to_string(Id);
	std::string& s1 = InputPins[0].ShaderIdentifier;
	ss << "vec4 " << m_Identifier << " = texture(" << m_TextureId << "," << s1 << ");\n";
	m_Initialised = true;
}

void TextureNode::ParseNodeLogic(std::stringstream& ss) {

}

std::string TextureNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier + OutputPins[outputPinIndex].ShaderAccessor;
}