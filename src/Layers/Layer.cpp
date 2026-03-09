#include "Layer.h"

namespace IcePick {

	void LayerManager::addLayer(std::shared_ptr<Layer> newLayer) {
		m_Layers.push_back(newLayer);
		newLayer->OnAttach();
	}

}