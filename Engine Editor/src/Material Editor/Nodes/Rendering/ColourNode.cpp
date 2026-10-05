#include "ColourNode.h"
#include "Public/EngineAPI.h"
#include "File Systems/AssetTypes.h"

ColourNode::ColourNode() {
	OutputPins.emplace_back(Pin::VEC4, "RGBA", ".rgba");
	OutputPins.emplace_back(Pin::VEC4, "RGB", ".rgb");
	OutputPins.emplace_back(Pin::FLOAT32, "R", ".r");
	OutputPins.emplace_back(Pin::FLOAT32, "G", ".g");
	OutputPins.emplace_back(Pin::FLOAT32, "B", ".b");
	OutputPins.emplace_back(Pin::FLOAT32, "A", ".a");

	m_Value = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_Identifier = "node_" + std::to_string(Id);
	m_NodeType = "colour";
	NodeWidth = 160.0f;
	nodeIsParameter = false;
	nodeCanBeParamterized = true;
	NodeName = "Colour";
	NodeHeaderColour = ImU32(0xFF917410);
}

void ColourNode::CustomRendering(IcePick::EngineAPI& engineAPI, std::filesystem::path& dropAssetPath, const NodeRenderInfo& renderInfo, ImVec2 canvasScreenPos, ImVec2 canvasScrolling) {
	if (nodeIsParameter)
		return;

	float nodeMaxHeight = renderInfo.PinYSpacing * OutputPins.size();
	float nodeMinHeightOffset = (renderInfo.PinYSpacing * 2) + renderInfo.PinRadius + renderInfo.LabelPadding;
	float maxLabelWidth = ImGui::CalcTextSize(OutputPins[0].Label.c_str()).x;
	ImVec2 buttonPos = ImVec2(CanvasPosition.x + canvasScreenPos.x + canvasScrolling.x + renderInfo.PinRadius + renderInfo.LabelPadding, CanvasPosition.y + canvasScreenPos.y + canvasScrolling.y + renderInfo.NodeHeaderHeight + renderInfo.NodePadding);
	ImVec2 buttonSize{ static_cast<float>(NodeWidth) - maxLabelWidth - (3 * renderInfo.LabelPadding) - (1 * renderInfo.NodePadding), (OutputPins.size() - 1) * renderInfo.PinYSpacing};
	
	ImGui::SetCursorScreenPos(buttonPos);
	bool openColourEditPopup = ImGui::ColorButton("##Colour Button", m_Value, 0, buttonSize);

	if (openColourEditPopup) {
		ImGui::OpenPopup("ColourNodeEditPopup");
	}

	if (ImGui::BeginPopup("ColourNodeEditPopup")) {
		ImGui::ColorPicker4("##ColourEdit", (float*)&m_Value);
		if (ImGui::IsItemDeactivatedAfterEdit()) {
			MaterialBaseStateChanged = true;
		}
		ImGui::EndPopup();
	}

}

void ColourNode::Initialise(std::stringstream& ss, IcePick::MaterialBase& editMaterialBase, IcePick::MaterialInstance& editMaterialInstance) {
	if (m_Initialised)
		return;

	if (!nodeIsParameter) {
		std::string sr = std::to_string(m_Value.x);
		std::string sg = std::to_string(m_Value.y);
		std::string sb = std::to_string(m_Value.z);
		std::string sa = std::to_string(m_Value.w);
		ss << "vec4 " << m_Identifier << " = vec4(" << sr << "," << sg << "," << sb << "," << sa << ");\n";
	}
	else {
		IcePick::MaterialBaseParameter materialBaseColourVec4Parameter{ NodeName, m_Identifier, Id };
		editMaterialBase.MaterialColourVec4Parameters.push_back(materialBaseColourVec4Parameter);

		IcePick::MaterialInstanceData<glm::vec4> materialInstanceColourVec4Parameter{ materialBaseColourVec4Parameter.Id, { m_Value.x, m_Value.y, m_Value.z, m_Value.w } };
		editMaterialInstance.InstanceColourVec4Data.push_back(materialInstanceColourVec4Parameter);
	}

	m_Initialised = true;
}

void ColourNode::ParseNodeLogic(std::stringstream& ss) {

}

std::string ColourNode::GetPinOutput(unsigned int outputPinIndex) {
	return m_Identifier + OutputPins[outputPinIndex].ShaderAccessor;
}