#include "IcePickRuntime.h"
#include "Layers/Layer.h"
#include "Layers/EngineLayer.h"
#include "Layers/EngineAPI.h"
#include "../Engine Editor/EditorLayer.h"
#include <iostream>
#include "Utilities/DebugStatistics.h"

using namespace IcePick;

bool Engine::Init() {
	IP_CORE_PROFILE_LOG("Temp", 6.9f);
	IP_CORE_PROFILE_BEGIN("Engine Init.");

	IP_CORE_PROFILE_BEGIN("Renderer Init.");
	if (!IcePickRenderer::InitRenderer()) {
		std::cout << "Engine renderer failed to initialise.\n";
		return false;
	}
	IP_CORE_PROFILE_POP(); // Renderer

	IP_CORE_PROFILE_BEGIN("Event Manager Init.");
	if (!IcePick::IP_EventHandler.Init()) {
		std::cout << "Event handler failed to initialise.\n";
		return false;
	}
	m_EngineRunning = true;
	IP_CORE_PROFILE_POP(); // Event Manager

	IP_CORE_PROFILE_BEGIN("Init Engine Layers.");
	std::shared_ptr<EngineLayer> engineLayer = std::make_shared<EngineLayer>();
#ifndef RELEASE
	std::shared_ptr<EditorLayer> editorLayer = std::make_shared<EditorLayer>(engineLayer);
	IP_LayerStack.addLayer(editorLayer);
#endif // !RELEASE
	IP_LayerStack.addLayer(engineLayer);
	IP_CORE_PROFILE_POP(); // Engine Layers

	IP_CORE_PROFILE_POP(); // Engine Init

	return true;
}

void Engine::Run() {

	DeltaTime deltaTime;
	while (m_EngineRunning) {
		deltaTime.NewFrame();

		if (IcePickRenderer::WindowShouldClose()) {
			Terminate();
			break;
		}
		glfwPollEvents();
		IcePickRenderer::NewFrame();

		for (auto& layer : IP_LayerStack.m_Layers) {
			layer->OnNewFrame();
		}

		IP_CORE_PROFILE_BEGIN("Update Layers");
		for (auto& layer : IP_LayerStack.m_Layers) {
			layer->OnUpdate(deltaTime);
		}
		IP_CORE_PROFILE_POP();

		auto layerIt = IP_LayerStack.m_Layers.rbegin();
		RenderPayload payload;
		for (; layerIt != IP_LayerStack.m_Layers.rend(); ++layerIt) {
			(*layerIt)->OnRender(payload);
		}

		IcePickRenderer::EndFrame();		
	}
}

void Engine::Terminate() {
	m_EngineRunning = false;
	for (auto layerIt = IP_LayerStack.m_Layers.rbegin(); layerIt != IP_LayerStack.m_Layers.rend(); ++layerIt) {
		(*layerIt)->OnDetach();
	}
	IcePickRenderer::TerminateRenderer();
	glfwTerminate();
}

