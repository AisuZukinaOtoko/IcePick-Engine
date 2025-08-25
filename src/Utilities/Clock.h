#pragma once
#include "Timer.h"

class DeltaTime {
public:
	// time in milliseconds since last frame
	DeltaTime();
	DeltaTime(const DeltaTime& other);
	void NewFrame();
	float GetDelta();
	float GetTrueDelta();
private:
	ManualTimer m_Timer;
	float m_DeltaTime = 0.0f;
};