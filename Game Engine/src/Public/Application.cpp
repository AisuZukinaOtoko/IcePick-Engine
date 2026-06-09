#include "../Render Systems/Renderer.h"
#include "Application.h"
#include "../Utilities/DebugStatistics.h"

namespace IcePick {
	void Application::AddLayer(std::shared_ptr<Layer> layer) {
		m_LayerStack.push_back(layer);
		layer->OnAttach();
	}

	void Application::PopLayer() {
		m_LayerStack.back()->OnDetach();

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
				break;
			}

			glfwPollEvents();
			IcePickRenderer::NewFrame();
			m_EventHandler.NewFrame();
			m_EventHandler.HandleEvents(m_LayerStack);

			for (auto layerIt = m_LayerStack.rbegin(); layerIt != m_LayerStack.rend(); ++layerIt) {
				(*layerIt)->OnNewFrame();
			}

			IP_CORE_PROFILE_BEGIN("Update Layers");
			for (auto layerIt = m_LayerStack.rbegin(); layerIt != m_LayerStack.rend(); ++layerIt) {
				(*layerIt)->OnUpdate(deltaTime);;
			}
			IP_CORE_PROFILE_POP();

			IP_CORE_PROFILE_BEGIN("Pre-Render Layers");
			for (auto layerIt = m_LayerStack.rbegin(); layerIt != m_LayerStack.rend(); ++layerIt) {
				(*layerIt)->OnPreRender();
			}
			IP_CORE_PROFILE_POP();

			RenderPayload payload;
			for (auto& layer : m_LayerStack) {
				layer->OnRender(payload);
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
		while (!m_LayerStack.empty()) {
			PopLayer();
		}
		IcePickRenderer::TerminateRenderer();
	}
}