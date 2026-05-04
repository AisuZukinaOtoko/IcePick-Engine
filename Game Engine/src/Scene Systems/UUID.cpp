#include "UUID.h"
#include <random>

static unsigned int IdCount = 0;

namespace IcePick {
	static std::random_device randomDevice;
	static std::mt19937_64 randomEngine(randomDevice());
	static std::uniform_int_distribution<uint64_t> uniformDistribution;

	UUID UUID::Unitialised() {
		return UUID((uint64_t)0);
	}

	UUID::UUID() {
		m_ID = uniformDistribution(randomEngine);
	}

	UUID::UUID(uint64_t value) {
		m_ID = value;
	}

	UUID::UUID(const UUID& other) {
		m_ID = other.m_ID;
	}
}