#include "Clock.h"

DeltaTime::DeltaTime() {
	m_DeltaTime = 0;
	m_Timer.begin();
}

DeltaTime::DeltaTime(const DeltaTime& other) {
	m_Timer = other.m_Timer;
	m_DeltaTime = other.m_DeltaTime;
}

void DeltaTime::NewFrame() {
	m_DeltaTime = (float)m_Timer.finish() / 1000.0f;
	m_Timer.begin();
}
float DeltaTime::GetDelta() {
	return m_DeltaTime;
}

float DeltaTime::GetTrueDelta() {
	return (float)m_Timer.finish() / 1000.0f;
}
