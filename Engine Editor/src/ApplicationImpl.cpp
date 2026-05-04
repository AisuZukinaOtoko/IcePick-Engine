#include "ApplicationImpl.h"
#include "Public/EngineLayer.h"
#include "EditorLayer.h"
#include "Utilities/DebugStatistics.h"
#include <iostream>

void EditorApplication::Init() {
	std::cout << "Hello World." << std::endl;

	IP_CORE_PROFILE_BEGIN("Engine Init.");

	IP_CORE_PROFILE_BEGIN("Renderer Init.");
	if (!IcePickRenderer::InitRenderer()) {
		std::cout << "Engine renderer failed to initialise.\n";
		return;
	}
	IP_CORE_PROFILE_POP(); // Renderer

	IP_CORE_PROFILE_BEGIN("Event Manager Init.");
	if (!m_EventHandler.Init()) {
		std::cout << "Event handler failed to initialise.\n";
		return;
	}
	IP_CORE_PROFILE_POP(); // Event Manager

	IP_CORE_PROFILE_BEGIN("Init Engine Layers.");
	std::shared_ptr<IcePick::EngineLayer> engineLayer = std::make_shared<IcePick::EngineLayer>();
	std::shared_ptr<IcePick::EditorLayer> editorLayer = std::make_shared<IcePick::EditorLayer>(engineLayer);

	AddLayer(engineLayer);
	IP_CORE_PROFILE_POP(); // Engine Layers

	IP_CORE_PROFILE_POP(); // Engine Init
}