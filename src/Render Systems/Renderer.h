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
	NewVertexArrayData AddVertexArray();
	//bool AddGeometry(IcePick::Mesh& mesh);
	//VertexArray& AddVertexArray();
}

