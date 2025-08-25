#include "Timer.h"
#include "../LogSystem.h"
#include <iostream>

ScopedTimer::ScopedTimer(const char* ID) {
	start = std::chrono::high_resolution_clock::now();
	this->ID = ID;
}

ScopedTimer::~ScopedTimer() {
	end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	IP_LOG("Timer " + std::string(ID) + ": " + std::to_string(duration.count()) + "ms.");
}

ManualTimer::ManualTimer(const char* ID) {
	start = std::chrono::high_resolution_clock::now();
	this->ID = ID;
}

ManualTimer::ManualTimer(const ManualTimer& other) {
	ID = other.ID;
	start = other.start;
}

void ManualTimer::begin() {
	start = std::chrono::high_resolution_clock::now();
}

long long ManualTimer::finish() {
	end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

ManualTimer::~ManualTimer() {
}