#include "IcePickRuntime.h"
#include "File Systems/FileManager.h"
#include "Scene Systems/SceneManager.h"
#include "Render Systems/Texture.h"
#include "Render Systems/VertexBuffer.h"
#include "Event Systems/EventHandler.h"
#include "Layers/Layer.h"
#include "Layers/GameLayer.h"
#include "../Engine Editor/EditorLayer.h"
#include <iostream>

using namespace IcePick;

bool Engine::Init() {
	if (!IcePickRenderer::InitRenderer()) {
		std::cout << "Engine renderer failed to initialise.\n";
		return false;
	}

	if (!IcePick::IP_EventHandler.Init()) {
		std::cout << "Event handler failed to initialise.\n";
		return false;
	}
	m_EngineRunning = true;
	IP_LayerStack.addLayer(std::make_shared<EditorLayer>());
	IP_LayerStack.addLayer(std::make_shared<GameLayer>());
	return true;
}

void Engine::Run() {

	while (m_EngineRunning) {
		GLCheckErrors();
		GLClearErrors();

		if (IcePickRenderer::WindowShouldClose()) {
			Terminate();
			break;
		}
		glfwPollEvents();

		IcePickRenderer::NewFrame();

		for (auto& layer : IP_LayerStack.m_Layers) {
			layer->OnUpdate(16.0f);
		}

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
	glfwTerminate();
}

