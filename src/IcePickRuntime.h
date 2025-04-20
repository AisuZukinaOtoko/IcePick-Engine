#pragma once
#include "Scene Systems/SceneManager.h"
#include "Render Systems/Renderer.h"
#include <vector>

namespace IcePick {

	class Engine {
	public:
		/*
		* 
		*/
		bool Init();
		//bool EnableEditor();
		void Run();
		void Terminate();

		/* 
		* @brief Ends current scene. Begins new scene.
		*/
		void BindScene(Scene& scene);
	private:
		void Render();
		//void NewFrame();
		//void EndFrame();

		Renderer m_Renderer;
		GLFWwindow* m_Window = nullptr;
		Scene* m_CurrentScene = nullptr;


		bool m_EngineRunning = false;
	};
}