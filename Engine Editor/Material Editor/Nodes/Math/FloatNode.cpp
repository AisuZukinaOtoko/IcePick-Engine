#include "FloatNode.h"

FloatNode::FloatNode() {
	OutputPins.emplace_back(Pin::FLOAT32, "Value", "");
	m_Identifier = "node_" + std::to_string(Id);
	nodeCanBeParamterized = true;
	NodeName = "Float";
	m_NodeType = "float";
}

void FloatNode::CustomRendering(IcePick::EngineAPI engineAPI, std::filesystem::path& dropAssetPath, const NodeRenderInfo& renderInfo, ImVec2 canvasScreenPos, ImVec2 canvasScrolling) {
	if (nodeIsParameter)
		return;

	ImVec2 inputPos = ImVec2(CanvasPosition.x + canvasScreenPos.x + canvasScrolling.x + renderInfo.LabelPadding, CanvasPosition.y + canvasScreenPos.y + canvasScrolling.y + renderInfo.NodeHeaderHeight);
	ImGui::SetCursorScreenPos(inputPos);
	ImGui::SetNextItemWidth(NodeWidth / 2.0f);
	ImGui::InputFloat("##FloatInput", &m_Value);
}

void FloatNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	if (nodeIsParameter) {
		
	}

	m_Initialised = true;
}

void FloatNode::ParseNodeLogic(std::stringstream& ss) {

}

std::string FloatNode::GetPinOutput(unsigned int outputPinIndex) {
	if (nodeIsParameter) {
		return m_Identifier;
	}

	return std::to_string(m_Value);
}