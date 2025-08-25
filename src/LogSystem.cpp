#include "LogSystem.h"

IcePick::Logger& IcePick::Logger::GetInstance() {
	static Logger GlobalLogger;
	return GlobalLogger;
}

void IcePick::Logger::Log(const char* message, int type) {
	m_LogHistory.emplace_back(message, type);
}

std::vector<IcePick::Log>* IcePick::Logger::GetLogHistory() {
	return &m_LogHistory;
}

void IcePick::Logger::ClearLog() {
	m_LogHistory.clear();
}
