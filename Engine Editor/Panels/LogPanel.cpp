#include "LogPanel.h"
#include "../../src/LogSystem.h"
#include "PanelCommon.h"

void LogPanel::RenderLogs() {
	auto LogHistory = IcePick::Logger::GetInstance().GetLogHistory();
	if (!LogHistory) {
		return;
	}

	ImGui::Begin(m_ID);

	if (ImGui::Button("Clear")) {
		ClearLogs();
		IP_LOG("Clear logs", IP_WARN_LOG);
	}
	
	
	for (const auto& Log : *LogHistory) {
		switch (Log.type) {
		case IP_WARN_LOG:
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
			break;
		case IP_ERROR_LOG:
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			break;
		default:
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			break;
		}
		ImGui::Text(Log.message.c_str());
		ImGui::PopStyleColor();
	}


	if (LogHistory->size() != m_CurrLogCount) { // Log entries have been added or removed
		m_CurrLogCount = LogHistory->size();
		ImGui::SetScrollHereY(1.0f);
	}
	ImGui::End();
}

void LogPanel::ClearLogs() {
	IcePick::Logger::GetInstance().ClearLog();
}

void LogPanel::RenderAllLogs() {
	auto LogHistory = IcePick::Logger::GetInstance().GetLogHistory();
}

void LogPanel::RenderPartialLogs(unsigned int startIndex, unsigned int endIndex) {
	auto LogHistory = IcePick::Logger::GetInstance().GetLogHistory();

}
