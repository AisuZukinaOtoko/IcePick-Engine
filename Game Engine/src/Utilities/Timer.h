#pragma once
#include <chrono> 

class ScopedTimer {
public:
	ScopedTimer(const char* ID = "Timer");
	ScopedTimer(const ScopedTimer& other) = delete;
	~ScopedTimer();

private:
	std::chrono::high_resolution_clock::time_point start, end;
	const char* ID = "";
};


class ManualTimer {
public:
	ManualTimer(const char* ID = "Timer");
	ManualTimer(const ManualTimer& other);
	~ManualTimer();
	void begin();
	long long finish();
private:
	std::chrono::high_resolution_clock::time_point start, end;
	const char* ID = "";
};