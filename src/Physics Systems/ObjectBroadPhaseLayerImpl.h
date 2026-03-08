#pragma once
#include "ObjectAndBroadPhaseLayers.h"

namespace IcePick {

	class ObjectBroadPhaseLayerImpl final : public JPH::BroadPhaseLayerInterface {
	public:
		ObjectBroadPhaseLayerImpl();
		virtual JPH::uint GetNumBroadPhaseLayers() const override;
		virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override;
	private:
		JPH::BroadPhaseLayer m_ObjectLayerToObjectBroadPhaseLayer[ObjectLayers::LAYER_COUNT];
	};
}