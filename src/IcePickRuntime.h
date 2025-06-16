#pragma once
#include "Scene Systems/SceneManager.h"
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
		Scene* m_CurrentScene = nullptr;

		bool m_EngineRunning = false;
	};
}