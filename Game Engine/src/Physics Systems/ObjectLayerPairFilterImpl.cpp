#include "ObjectLayerPairFilterImpl.h"
#include "../Utilities/Assert.h"

namespace IcePick {
	bool ObjectLayerPairFilterImpl::ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const {
		return true;
		switch (inObject1) {
		case ObjectLayers::NON_MOVING:
			return inObject2 == ObjectLayers::MOVING; // Non moving only collides with moving
		case ObjectLayers::MOVING:
			return true; // Moving object
		default:
			IP_ASSERT(false, "Invalid object layer.");
			return false;
		}
	}
}