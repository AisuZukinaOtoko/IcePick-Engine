#pragma once
#include <glm/glm.hpp>

namespace IcePick {
	struct ColliderShape {
		glm::vec3 ColliderOffset = glm::vec3(0.0f);
		glm::vec3 ColliderScale = glm::vec3(1.0f);
		float Radius = 0.5f;

		enum class ColliderShapeType {
			BOX_SHAPE = 0,
			SPHERE_SHAPE,
			CAPSULE_SHAPE,
			STATIC_COMPOUND_SHAPE,
			COLLIDER_SHAPE_COUNT
		} ShapeType = ColliderShapeType::BOX_SHAPE;
	};
}
