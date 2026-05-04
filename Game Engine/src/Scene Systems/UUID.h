#pragma once
#include <stdint.h>
#include <cstddef>
#include <functional>


namespace IcePick {
	class UUID {
	public:
		UUID();
		UUID(uint64_t value);
		UUID(const UUID& other);
		~UUID() = default;

		static UUID Unitialised();
		operator uint64_t() const { return m_ID;  }
		bool operator==(const UUID& other) { return m_ID == other.m_ID; }
		operator uint64_t() { return m_ID; }
	private:
		uint64_t m_ID;
	};

}

// Hashing function for IcePick UUIDs
struct UUIDHasher {
	std::size_t operator()(const IcePick::UUID& uuid) const noexcept {
		return std::hash<uint64_t>()(static_cast<uint64_t>(uuid));
	}
};