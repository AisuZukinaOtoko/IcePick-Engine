#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <iostream>
#include "Vertex.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "FrameBuffer.h"
#include "../Vendor/imgui-docking/imgui.h"
#include "../Vendor/glm/glm.hpp"
#include "ShaderProgram.h"


namespace  IcePick {
	struct MeshComponent;
};

namespace IcePickRenderer {
	bool InitRenderer();
	void TerminateRenderer();
	void NewFrame();
	void EndFrame();
	void DrawMesh(const IcePick::MeshComponent& mesh, glm::mat4 modelTransformMatrix, IcePick::ShaderProgram& shaderProgram);
	void UpdateRenderViewProjectionMatrix(const glm::mat4 vpm);

	bool WindowShouldClose();
	GLFWwindow* GetRendererWindow();
	glm::ivec2 GetRendererWindowSize();
	bool IsCursorLocked();
	void RequestCursorLock();
	void RequestCursorUnlock();
	glm::vec2 GetMousePos();
	glm::vec2 GetMouseDelta();

	void SetDrawLineShader(const IcePick::ShaderProgram& lineShaderProgram);
	void DrawLine(const LinePointVertex3D& point1, const LinePointVertex3D& point2);
	void FlushLineRenderBuffer();
	
	// Render camera settings
	void SetRenderCameraWorldPosition(glm::vec3 CameraWorldPosition);
	void SetRenderViewProjectionMatrix(glm::mat4 ViewProjectionMatrix);
	void SetRenderWorldNormalMatrix(glm::mat3 WorldNormalMatrix);
	NewVertexArrayData AddVertexArray();
}

