#pragma once
#include <vector>
#include <string>

#define IP_STANDARD_LOG 0
#define IP_WARN_LOG 1
#define IP_ERROR_LOG 2

namespace IcePick {
	struct Log {
		Log(const char* msg, int type) : message(msg), type(type) {};
		std::string message;
		int type;
	};

	class Logger {
	public:
		static Logger& GetInstance();
		void Log(const char* message, int type);
		std::vector<IcePick::Log>* GetLogHistory();
		void ClearLog();
	private:
		std::vector<IcePick::Log> m_LogHistory;
	};
}

inline void IP_LOG(std::string log, int type = IP_STANDARD_LOG) {
#ifndef DIST
	IcePick::Logger::GetInstance().Log(log.c_str(), type);
#endif
}