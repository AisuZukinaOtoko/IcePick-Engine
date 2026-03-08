#include "ObjectVsBroadPhaseLayerImpl.h"
#include "../Utilities/Assert.h"

namespace IcePick {
	bool ObjectVsBroadPhaseLayerImpl::ShouldCollide(JPH::ObjectLayer objectLayer, JPH::BroadPhaseLayer broadPhaseLayer) const {
		return true;
		switch (objectLayer) {
		case ObjectLayers::NON_MOVING:
			return broadPhaseLayer == BroadPhaseLayers::MOVING;
		case ObjectLayers::MOVING:
			return true;
		default:
			IP_ASSERT(false, "Invalid object layer.");
			return false;
		}
	}
}