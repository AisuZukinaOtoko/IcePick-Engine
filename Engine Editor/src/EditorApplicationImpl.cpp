#include "EditorApplicationImpl.h"
#include "Public/EngineLayer.h"
#include "EditorLayer.h"
#include "Utilities/DebugStatistics.h"
#include <iostream>

void EditorApplication::Init() {
	if (!IcePickRenderer::InitRenderer()) {
		std::cout << "Engine renderer failed to initialise." << std::endl;
		return;
	}

	if (!m_EventHandler.Init()) {
		std::cout << "Event handler failed to initialise." << std::endl;
		return;
	}
	std::shared_ptr<IcePick::EngineLayer> engineLayer = std::make_shared<IcePick::EngineLayer>();
	std::shared_ptr<IcePick::EditorLayer> editorLayer = std::make_shared<IcePick::EditorLayer>(engineLayer);

	AddLayer(engineLayer);
	AddLayer(editorLayer);
	m_ApplicationRunning = true;
}