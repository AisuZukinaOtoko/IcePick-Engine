#pragma once

class DeltaTime {
public:
	// time in milliseconds since last frame
	DeltaTime(float time) : m_DeltaTime(time) {}
	float GetDelta() { return m_DeltaTime; }
	float GetTrueDelta() { return m_DeltaTime;  };
private:
	float m_DeltaTime = 0.0f;
};