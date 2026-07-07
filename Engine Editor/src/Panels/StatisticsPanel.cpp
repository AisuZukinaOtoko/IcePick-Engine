#include "StatisticsPanel.h"
#include "PanelCommon.h"
#include "Utilities/DebugStatistics.h"

StatisticsPanel::StatisticsPanel() {
	m_Title = "Statistics";
}

void StatisticsPanel::ShowStats() {
	ImGui::Begin(m_Title);
	const DebugCapture& rootDebugCapture = IP_GET_CORE_PROFILE();
	for (const DebugCapture& childDebugCapture : rootDebugCapture.ChildCaptures) {
		RenderNodeRecursive(childDebugCapture);
	}
	ImGui::End();
}

void StatisticsPanel::RenderNodeRecursive(const DebugCapture& parentDebugCapture) {
	ImGuiTreeNodeFlags entityNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Leaf;
	if (ImGui::TreeNodeEx(parentDebugCapture.PropertyName.c_str(), entityNodeFlags)) {
		ImGui::SameLine(220);
		ImGui::Text(parentDebugCapture.PropertyValue.c_str());

		for (const DebugCapture& childDebugCapture : parentDebugCapture.ChildCaptures) {
			RenderNodeRecursive(childDebugCapture);
		}
		ImGui::TreePop();
	}	
}

StatisticsPanel::~StatisticsPanel() {

}