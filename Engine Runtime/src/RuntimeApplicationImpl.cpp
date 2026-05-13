#include "RuntimeApplicationImpl.h"
#include "Public/EngineLayer.h"
#include "Scene Systems/Components.h"
#include "RuntimeLayer.h"
#include <iostream>

void RuntimeApplication::Init() {
	if (!IcePickRenderer::InitRenderer()) {
		std::cout << "Engine renderer failed to initialise." << std::endl;
		return;
	}

	if (!m_EventHandler.Init()) {
		std::cout << "Event handler failed to initialise." << std::endl;
		return;
	}

	std::shared_ptr<IcePick::EngineLayer> engineLayer = std::make_shared<IcePick::EngineLayer>();
	std::shared_ptr<RuntimeLayer> runtimeLayer = std::make_shared<RuntimeLayer>(engineLayer);

	AddLayer(engineLayer);
	AddLayer(runtimeLayer);
	m_ApplicationRunning = true;
}