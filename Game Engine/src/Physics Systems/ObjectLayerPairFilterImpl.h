#pragma once
#include "ObjectAndBroadPhaseLayers.h"

namespace IcePick {
	class ObjectLayerPairFilterImpl final : public JPH::ObjectLayerPairFilter {
	public:
		virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override;
	private:
	};
}