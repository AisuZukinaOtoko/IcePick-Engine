#pragma once
#include <stdint.h>

struct SelectionContext {
	enum class Type {
		ENTITY = 0,
		BONE
	} SelectionType{ Type::ENTITY };
	uint32_t SelectionId{ 0 };
	uint32_t SelectionData{ 0 };
};