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
	/*if (!m_Renderer.Init()) {
		std::cout << "Engine renderer failed to initialise.\n";
		return false;
	}*/
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

//bool IcePick::Engine::EnableEditor(){
//	//return m_Renderer.InitForImgui();
//}

void Engine::Run() {
	unsigned int meshID = GlobalFileSystem.LoadMesh<FBX>("res/Assets/cube.fbx");
	std::cout << "Loaded mesh" << std::endl;
	m_Renderer.AddGeometry(GlobalFileSystem.GetMesh(meshID));


	//float positions[] = { // vertex coords vec3, texture coords vec2
	//	-0.4f,  0.2f, 0.5f, 0.0f, 1.0f,
	//	-1.0f, -1.0f, 0.5f, 0.0f, 0.0f,
	//	 1.0f, -1.0f, 0.5f, 1.0f, 0.0f,
	//	 0.4f,  0.2f, 0.5f, 1.0f, 1.0f
	//};

	//unsigned int indices[] = {
	//	3, 0, 1,
	//	1, 2, 3
	//};

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Texture texture("res/textures/CatWithHat.png");
	//texture.Bind(0);

	//unsigned int vao;
	//glGenVertexArrays(1, &vao);
	//glBindVertexArray(vao);

	//VertexArray vertexArray;
	//VertexBuffer vertexBuffer(positions, 5 * 4 * sizeof(float));
	//VertexBufferLayout layout;
	//layout.Push<float>(3); // change this to reflect vertex buffer layout
	//layout.Push<float>(2);
	//vertexArray.AddBuffer(vertexBuffer, layout);
	//IndexBuffer indexBuffer(indices, 6);

	while (m_EngineRunning) {
		GLCheckErrors();
		GLClearErrors();
		/*if (m_Renderer.WindowShouldClose()) {
			Terminate();
			break;
		}*/
		if (IcePickRenderer::WindowShouldClose()) {
			Terminate();
			break;
		}
		glfwPollEvents();

		IcePickRenderer::NewFrame();
		//m_Renderer.BeginFrame();
		//m_Renderer.UpdateViewportSize(m_CurrentScene->GetViewportSize());
		//vertexArray.Bind();
		//glDrawElements(GL_TRIANGLES, indexBuffer.GetCount(), GL_UNSIGNED_INT, nullptr);
		for (auto& layer : IP_LayerStack.m_Layers) {
			layer->OnUpdate(16.0f);
		}

		auto layerIt = IP_LayerStack.m_Layers.rbegin();
		RenderPayload payload;
		for (; layerIt != IP_LayerStack.m_Layers.rend(); ++layerIt) {
			(*layerIt)->OnRender(payload);
		}

		//int width, height;
		//glfwGetWindowSize(Renderer::Window, &width, &height);
		//glViewport(0, 0, width, height);
		//m_CurrentScene->OnUpdate(16.0f);
		//m_Renderer.Draw();
		//m_CurrentScene->OnUIRender();

		IcePickRenderer::EndFrame();
		//m_Renderer.EndFrame();
	}

	//if (m_CurrentScene)
	//	m_CurrentScene->OnEnd();

}

void IcePick::Engine::BindScene(Scene& scene){
	/*if (m_CurrentScene)
		m_CurrentScene->OnEnd();

	m_CurrentScene = &scene;
	m_CurrentScene->OnBegin();*/
	//IP_EventHandler.ClearEvents();
}

void Engine::Render() {

}

void Engine::Terminate() {
	m_EngineRunning = false;
	glfwTerminate();
}

