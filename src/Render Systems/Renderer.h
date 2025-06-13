#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <iostream>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "FrameBuffer.h"
#include "../Scene Systems/Components.h"
#include "../File Systems/FileDefinitions.h"
#include "../Vendor/imgui-docking/imgui.h"
#include "../Vendor/glm/glm.hpp"


void GLCheckErrors();
void GLClearErrors();

namespace IcePickRenderer {
	bool InitRenderer();
	void NewFrame();
	void EndFrame();
	//void CreateShaderPogram(std::string shaderSource);
	void DrawMesh(const IcePick::MeshComponent& mesh, glm::mat4 modelTransformMatrix);
	void DrawMeshBasicMaterial(const IcePick::MeshComponent& mesh, const glm::mat4& modelTransformMatrix);
	void UpdateRenderViewProjectionMatrix(const glm::mat4 vpm);

	bool WindowShouldClose();
	GLFWwindow* GetRendererWindow();
	glm::ivec2 GetRendererWindowSize();
	void RequestCursorLock();
	void RequestCursorUnlock();
	
	// Render camera settings
	void SetRenderCameraWorldPosition(glm::vec3 CameraWorldPosition);
	void SetRenderViewProjectionMatrix(glm::mat4 ViewProjectionMatrix);
	void SetRenderWorldNormalMatrix(glm::mat3 WorldNormalMatrix);
	VertexArray& AddVertexArray();
	//bool AddGeometry(IcePick::Mesh& mesh);
}

namespace IcePick {
	class Renderer {
	public:
		Renderer();
		~Renderer();

		//bool Init();
		//bool InitForImgui();
		//bool WindowShouldClose();
		bool AddGeometry(Mesh& mesh);
		//void BeginFrame();
		//void EndFrame();
		//void Clear();
		//void UpdateViewportSize(ImVec2);
		//void Draw();

		//static GLFWwindow* Window;
		//static int WindowWidth;
		//static int WindowHeight;
	private:
		// Number of draw calls per frame
		//unsigned int m_DrawCallsCount;
		//unsigned int m_ShaderProgram;
		//void DrawGeometry(const VertexArray& va);
		//void CompileShader(std::string source);
		//std::vector<VertexArray> m_VertArrRegistry;
		//std::vector<FrameBuffer> m_FrameBuffers;
	};
}

