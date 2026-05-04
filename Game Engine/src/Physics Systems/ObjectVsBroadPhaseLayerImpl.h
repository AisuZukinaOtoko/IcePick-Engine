#pragma once
#include "ObjectAndBroadPhaseLayers.h"

namespace IcePick {
	class ObjectVsBroadPhaseLayerImpl final : public JPH::ObjectVsBroadPhaseLayerFilter {
	public:
		virtual bool ShouldCollide(JPH::ObjectLayer objectLayer, JPH::BroadPhaseLayer broadPhaseLayer) const override;
	private:
	};
}