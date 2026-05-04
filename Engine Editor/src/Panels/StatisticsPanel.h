#pragma once
#include "Utilities/DebugStatistics.h"

class StatisticsPanel {
public:
	StatisticsPanel();
	~StatisticsPanel();
	void ShowStats();
private:
	void RenderNodeRecursive(const DebugCapture& debugCapture);
	const char* m_Title;
};