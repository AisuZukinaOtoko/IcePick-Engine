#pragma once
#include "Render Systems/Renderer.h"
#include <vector>

namespace IcePick {

	class Engine {
	public:
		bool Init();
		void Run();
		void Terminate();
	private:
		GLFWwindow* m_Window = nullptr;

		bool m_EngineRunning = false;
	};
}