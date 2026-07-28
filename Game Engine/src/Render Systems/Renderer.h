#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct NewVertexArrayData;

namespace  IcePick {
	struct MeshComponent;
	class ShaderProgram;
};

namespace IcePickRenderer {
	struct LinePointVertex3D;
	class VertexArray;

	bool InitRenderer();
	void TerminateRenderer();
	void NewFrame();
	void EndFrame();
	void DrawMesh(IcePickRenderer::VertexArray& vertexArray, glm::mat4 modelTransformMatrix, IcePick::ShaderProgram& shaderProgram);
	void UpdateRenderViewProjectionMatrix(const glm::mat4 vpm);

	bool WindowShouldClose();
	GLFWwindow* GetRendererWindow();
	void SetCurrentContext(GLFWwindow* window);
	glm::ivec2 GetRendererWindowSize();
	bool IsCursorLocked();
	void RequestCursorLock();
	void RequestCursorUnlock();
	glm::vec2 GetMousePos();
	glm::vec2 GetMouseDelta();

	void SetDrawLineShader(const IcePick::ShaderProgram& lineShaderProgram);
	void DrawLine(const LinePointVertex3D& point1, const LinePointVertex3D& point2);
	void FlushLineRenderBuffer();

	void EnableDepthTesting();
	void DisableDepthTesting();
	void EnableDepthWrites();
	void DisableDepthWrites();
	void EnableBackFaceCulling();
	void DisableBackFaceCulling();
	void FullScreenPass();
	
	// Render camera settings
	void SetRenderCameraWorldPosition(glm::vec3 CameraWorldPosition);
	void SetRenderViewProjectionMatrix(glm::mat4 ViewProjectionMatrix);
	void SetRenderWorldNormalMatrix(glm::mat3 WorldNormalMatrix);
	//NewVertexArrayData AddVertexArray();
}

