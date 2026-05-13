#pragma once
#include "../Event Systems/Event.h"
#include "../Utilities/Clock.h"
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
		virtual void OnNewFrame() {};
		virtual void OnPreRender() {};
		virtual void OnRender(RenderPayload& payload) {};
		bool m_Active = true;
	private:
	};

}
