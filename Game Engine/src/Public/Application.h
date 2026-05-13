#pragma once
#include "../Event Systems/EventHandler.h"
#include "Layer.h"
#include <vector>

namespace IcePick {
	class Application {
	public:
		virtual void Init() = 0;
		void Run();
		void Terminate();
	protected:
		bool m_ApplicationRunning = false; 
		void AddLayer(std::shared_ptr<Layer> layer);
		void PopLayer();
		std::vector<std::shared_ptr<Layer>> m_LayerStack;
		EventHandler m_EventHandler;
	private:
	};
}