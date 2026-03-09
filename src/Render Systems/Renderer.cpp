#include <iostream>
#include <fstream>
#include <sstream>
#include "Renderer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "RenderLineBuffer.h"
#include "../Vendor/glm/glm.hpp"
#include "../Vendor/glm/gtc/matrix_transform.hpp"
#include "../../src/LogSystem.h"
#include "../Scene Systems/Components.h"
#include "../Utilities/Assert.h"

using namespace IcePick;

void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length,
	const GLchar* message, const void* userParam) {
	IP_LOG(message, IP_ERROR_LOG);
}

static void OnWindowResize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	IP_LOG("Window Resized");
}

namespace IcePickRenderer {
	static GLFWwindow* MainTargetWindow = nullptr;
	static glm::ivec2 MainTargetWindowSize;
	static glm::vec2 CurrentFrameMousePosition;
	static glm::vec2 MouseDelta;
	static glm::vec3 CameraPosition;
	static glm::mat4 RenderViewProjectionMatrix;
	static glm::mat3 RenderWorldNormalMatrix; // No translation
	static std::vector<VertexArray> VertexArrays;

	static unsigned int InternalLineVertexArrayId = 0;
	static unsigned int InternalLineBufferId = 0;
	static RenderLineBuffer LineBuffer;

	static unsigned int BasicMaterialShaderID = 0;

	bool InitRenderer() {
		if (!glfwInit())
			return false;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);


		MainTargetWindow = glfwCreateWindow(1280, 960, "IcePick Engine", NULL, NULL);
		if (!MainTargetWindow)
			return false;

		glfwMakeContextCurrent(MainTargetWindow);
		glfwSwapInterval(1);
		glfwMaximizeWindow(MainTargetWindow);
		glfwGetWindowSize(MainTargetWindow, &MainTargetWindowSize[0], &MainTargetWindowSize[1]);
		IP_ASSERT((MainTargetWindowSize[0] != 0 && MainTargetWindowSize[1] != 0), "Invalid window size");
		glViewport(0, 0, MainTargetWindowSize[0], MainTargetWindowSize[1]);
		glfwSetFramebufferSizeCallback(MainTargetWindow, OnWindowResize);

		// Init glew
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (GLEW_OK != err) {
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			return false;
		}
		
		std::cout << "OpenGL version: " << glGetString(GL_VERSION) << "\n";
		std::cout << "OpenGL shading language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		// Line Rendering Setup
		glGenVertexArrays(1, &InternalLineVertexArrayId);
		glBindVertexArray(InternalLineVertexArrayId);

		glGenBuffers(1, &InternalLineBufferId);
		glBindBuffer(GL_ARRAY_BUFFER, InternalLineBufferId);
		glBufferData(GL_ARRAY_BUFFER, LineBuffer.MaxLinePointCount * sizeof(LinePointVertex3D), nullptr, GL_DYNAMIC_DRAW);

		const VertexLayout& layout = LinePointVertex3D::GetVertexLayout();
		const auto& elements = layout.GetElements();
		for (unsigned int i = 0; i < elements.size(); i++) {
			const IcePickRenderer::VertexBufferElement& element = elements[i];
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)element.offset);
		}

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// OpenGL Debug Callbacks
		glDebugMessageCallback(debugCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
		glDebugMessageControl(GL_DONT_CARE,	GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);


		VertexArrays.reserve(100);
		return true;
	}

	void NewFrame() {
		double x, y;
		glfwGetCursorPos(MainTargetWindow, &x, &y);
		MouseDelta = glm::vec2(x, y) - CurrentFrameMousePosition;
		CurrentFrameMousePosition = glm::vec2(x, y);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void EndFrame()	{
		glfwSwapBuffers(MainTargetWindow);
	}

	void UpdateRenderViewProjectionMatrix(const glm::mat4 vpm) {
		RenderViewProjectionMatrix = vpm;
	}

	bool WindowShouldClose() {
		return glfwWindowShouldClose(MainTargetWindow);
	}

	GLFWwindow* GetRendererWindow()	{
		return MainTargetWindow;
	}

	glm::ivec2 GetRendererWindowSize() {
		return MainTargetWindowSize;
	}

	bool IsCursorLocked() {
		return (glfwGetInputMode(MainTargetWindow, GLFW_CURSOR) == GLFW_CURSOR_DISABLED);
	}

	void RequestCursorLock() {
		glfwSetInputMode(MainTargetWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void RequestCursorUnlock() {
		glfwSetInputMode(MainTargetWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	glm::vec2 GetMousePos() {
		return CurrentFrameMousePosition;
	}

	glm::vec2 GetMouseDelta() {
		return MouseDelta;
	}

	void DrawLine(const LinePointVertex3D& point1, const LinePointVertex3D& point2, IcePick::ShaderProgram& shaderProgram) {
		if (LineBuffer.NumPoints + 2 > LineBuffer.MaxLinePointCount) {
			FlushLineRenderBuffer(shaderProgram);
		}

		LineBuffer.LinePoints[LineBuffer.NumPoints] = point1;
		LineBuffer.NumPoints++;
		LineBuffer.LinePoints[LineBuffer.NumPoints] = point2;
		LineBuffer.NumPoints++;
	}

	void FlushLineRenderBuffer(IcePick::ShaderProgram& shaderProgram) {
		if (LineBuffer.NumPoints == 0)
			return;

		shaderProgram.SetUniformMat4("u_ViewProjectionMatrix", RenderViewProjectionMatrix);
		shaderProgram.Use();

		glBindVertexArray(InternalLineVertexArrayId);
		glBindBuffer(GL_ARRAY_BUFFER, InternalLineBufferId);
		glBufferSubData(GL_ARRAY_BUFFER, 0,	LineBuffer.NumPoints * sizeof(LinePointVertex3D), LineBuffer.LinePoints);
		glDrawArrays(GL_LINES, 0, LineBuffer.NumPoints);

		shaderProgram.UnBind();
		LineBuffer.NumPoints = 0;
	}

	void SetRenderCameraWorldPosition(glm::vec3 CameraWorldPosition) {
		CameraPosition = CameraWorldPosition;
	}

	void SetRenderViewProjectionMatrix(glm::mat4 ViewProjectionMatrix) {
		RenderViewProjectionMatrix = ViewProjectionMatrix;
	}

	void SetRenderWorldNormalMatrix(glm::mat3 WorldNormalMatrix) {
		RenderWorldNormalMatrix = WorldNormalMatrix;
	}

	void DrawMesh(const MeshComponent& mesh, glm::mat4 modelTransformMatrix, IcePick::ShaderProgram& shaderProgram) {
		glm::mat4 MVP = RenderViewProjectionMatrix * modelTransformMatrix;

		//unsigned int shaderId = shaderProgram.GetID();
		//int location = glGetUniformLocation(shaderId, "u_MVP"); // location negative if uniform not found
		//glUniformMatrix4fv(location, 1, GL_FALSE, &MVP[0][0]);
		//location = glGetUniformLocation(shaderId, "u_Modelmatrix"); // location negative if uniform not found
		//glUniformMatrix4fv(location, 1, GL_FALSE, &modelTransformMatrix[0][0]);
		//location = glGetUniformLocation(shaderId, "u_NormalMatrix"); // location negative if uniform not found
		//glUniformMatrix3fv(location, 1, GL_FALSE, &RenderWorldNormalMatrix[0][0]);
		//location = glGetUniformLocation(shaderId, "u_CameraPosition"); // location negative if uniform not found
		//glUniform3fv(location, 1, &CameraPosition[0]);
		shaderProgram.SetUniformMat4("u_MVP", MVP);
		shaderProgram.SetUniformMat4("u_Modelmatrix", modelTransformMatrix);
		shaderProgram.SetUniformMat3("u_NormalMatrix", RenderWorldNormalMatrix);
		shaderProgram.SetUniformVec3("u_CameraPosition", CameraPosition);

		shaderProgram.Use();
		VertexArray& meshVertexArray = VertexArrays[mesh.MeshVertexArrayRegistryIndex];
		meshVertexArray.Bind();
		glDrawElements(GL_TRIANGLES, meshVertexArray.IndexCount, GL_UNSIGNED_INT, nullptr);
		meshVertexArray.Unbind();
		shaderProgram.UnBind();
	}

	NewVertexArrayData AddVertexArray() {
		VertexArrays.emplace_back();
		return { VertexArrays.back(), static_cast<unsigned int>(VertexArrays.size() - 1) };
	}

	void TerminateRenderer() {
		for (VertexArray va : VertexArrays) {
			va.Destroy();
		}

		glDeleteBuffers(1, &InternalLineBufferId);
	}
}
