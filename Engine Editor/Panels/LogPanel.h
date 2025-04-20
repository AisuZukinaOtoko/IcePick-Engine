#pragma once


class LogPanel {
public:
	void RenderLogs();
	void ClearLogs();
private:
	void RenderAllLogs();
	void RenderPartialLogs(unsigned int startIndex, unsigned int endIndex);
	const char* m_ID = "Debug Log";
	unsigned int m_CurrLogCount = 0; // current number of logs in the panel.
};