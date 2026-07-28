#pragma once
#include <stdint.h>

struct SelectionContext {
	enum class Type {
		ENTITY = 0,
		BONE
	} SelectionType{ Type::ENTITY };
	uint64_t SelectionId{ 0 };
	uint64_t SelectionData{ 0 };
};