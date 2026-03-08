#include "ObjectBroadPhaseLayerImpl.h"
#include "../Utilities/Assert.h"

namespace IcePick {
	ObjectBroadPhaseLayerImpl::ObjectBroadPhaseLayerImpl() {
		m_ObjectLayerToObjectBroadPhaseLayer[ObjectLayers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
		m_ObjectLayerToObjectBroadPhaseLayer[ObjectLayers::MOVING] = BroadPhaseLayers::MOVING;
	}

	JPH::uint ObjectBroadPhaseLayerImpl::GetNumBroadPhaseLayers() const {
		return BroadPhaseLayers::LAYER_COUNT;
	}

	JPH::BroadPhaseLayer ObjectBroadPhaseLayerImpl::GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const {
		IP_ASSERT(inLayer < ObjectLayers::LAYER_COUNT, "Invalid object layer.");
		return m_ObjectLayerToObjectBroadPhaseLayer[inLayer];
	}
}