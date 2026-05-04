#include "Application.h"
#include "../Utilities/DebugStatistics.h"

namespace IcePick {
	void Application::AddLayer(std::shared_ptr<Layer> layer) {
		m_LayerStack.push_back(layer);
		layer->OnAttach();
	}

	void Application::PopLayer() {
		if (m_LayerStack.empty())
			return;

		m_LayerStack.pop_back();
	}

	void Application::Run() {
		DeltaTime deltaTime;
		
		while (m_ApplicationRunning) {
			IP_CORE_PROFILE_CLEAR();
			IP_CORE_PROFILE_BEGIN("Total frame time");
			deltaTime.NewFrame();

			if (IcePickRenderer::WindowShouldClose()) {
				Terminate();
				break;
			}
			glfwPollEvents();
			IcePickRenderer::NewFrame();
			m_EventHandler.NewFrame();
			m_EventHandler.HandleEvents(m_LayerStack);

			for (auto& layer : m_LayerStack) {
				layer->OnNewFrame();
			}

			IP_CORE_PROFILE_BEGIN("Update Layers");
			for (auto& layer : m_LayerStack) {
				layer->OnUpdate(deltaTime);
			}
			IP_CORE_PROFILE_POP();

			IP_CORE_PROFILE_BEGIN("Pre-Render Layers");
			for (auto& layer : m_LayerStack) {
				layer->OnPreRender();
			}
			IP_CORE_PROFILE_POP();

			auto layerIt = m_LayerStack.rbegin();
			RenderPayload payload;
			for (; layerIt != m_LayerStack.rend(); ++layerIt) {
				(*layerIt)->OnRender(payload);
			}

			IP_CORE_PROFILE_BEGIN("Swap Buffers");
			IcePickRenderer::EndFrame();
			IP_CORE_PROFILE_POP();

			IP_CORE_PROFILE_POP();
			IP_CORE_PROFILE_CAPTURE();
		}
	}

	void Application::Terminate() {
		m_ApplicationRunning = false;
		for (auto layerIt = m_LayerStack.rbegin(); layerIt != m_LayerStack.rend(); ++layerIt) {
			(*layerIt)->OnDetach();
		}
		IcePickRenderer::TerminateRenderer();
	}
}