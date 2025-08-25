#pragma once
#include "../Utilities/Clock.h"
#include "../Event Systems/EventHandler.h"
#include <vector>
#include <memory>

namespace IcePick {
	struct RenderPayload {
		unsigned int FrameBufferID = 0;
	};

	class Layer {
	public:
		Layer() = default;
		Layer(const Layer&) = default;
		~Layer() = default;
		virtual void OnAttach() = 0;
		virtual void OnUpdate(DeltaTime dt) = 0;
		virtual void OnDetach() = 0;

		virtual void OnEvent(Event& event) {};
		virtual void OnRender(RenderPayload& payload) {};
		bool m_Active = true;
	private:
		//void BindScene(Scene& scene);
	};

	class LayerManager {
	public:
		void addLayer(std::shared_ptr<Layer> newLayer);
		std::vector<std::shared_ptr<Layer>> m_Layers;
	private:
	};
	inline LayerManager IP_LayerStack;
}
