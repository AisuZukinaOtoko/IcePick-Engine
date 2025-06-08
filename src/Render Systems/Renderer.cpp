#include <iostream>
#include <fstream>
#include <sstream>
#include "Renderer.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Materials.h"
#include "../Vendor/imgui-docking/imgui.h"
#include "../Vendor/imgui-docking/imgui_impl_glfw.h"
#include "../Vendor/imgui-docking/imgui_impl_opengl3.h"
#include "../Vendor/glm/glm.hpp"
#include "../Vendor/glm/gtc/matrix_transform.hpp"
#include "../../src/LogSystem.h"
#include "../Scene Systems/Components.h"
#include "../Utilities/Assert.h"


using namespace IcePick;

static void OnWindowResize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	IP_LOG("Window Resized");
}

struct ShaderProgramSource {
	std::string vertexSource;
	std::string fragmentSource;
};

static ShaderProgramSource parseShader(const std::string filepath) {
	std::ifstream inFile(filepath);
	if (inFile.fail()) {
		std::cerr << "Error: Could not open shader file." << std::endl;
		return {};
	}

	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;
	while (getline(inFile, line)) {
		if (line.find("shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) {
				type = ShaderType::FRAGMENT;
			}
		}
		else {
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

static unsigned int compileShader(unsigned int type, const std::string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// TODO: Error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* err = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, err);
		std::cout << "Failed to compile shader.\n";
		std::cout << err << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vShader = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fShader = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vShader);
	glAttachShader(program, fShader);
	glLinkProgram(program);
	glValidateProgram(program);

	// TODO: Error handling

	glDeleteShader(vShader);
	glDeleteShader(fShader);

	return program;
}


namespace IcePickRenderer {
	static GLFWwindow* MainTargetWindow = nullptr;
	static glm::ivec2 MainTargetWindowSize;
	static glm::mat4 RenderViewProjectionMatrix;
	static std::vector<VertexArray> VertexArrays;

	static unsigned int BasicMaterialShaderID = 0;
	IcePick::Material BasicMaterial;

	bool InitRenderer() {
		if (!glfwInit())
			return false;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


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
		GLenum err = glewInit();
		if (GLEW_OK != err) {
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			return false;
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		VertexArrays.reserve(15);


		float fov = glm::radians(45.0f);
		float aspectRatio = MainTargetWindowSize[0] / (float)MainTargetWindowSize[1];
		float nearClip = 0.1f;
		float farClip = 100.0f;
		float angle = glm::radians(45.0f);
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
		// Perspective matrix
		glm::mat4 projection = glm::perspective(fov, aspectRatio, nearClip, farClip);
		// View matrix
		glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		// Initialise model matrix
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		RenderViewProjectionMatrix = projection * view;
		// MVP matrix
		glm::mat4 mvp = projection * view * model;



		ShaderProgramSource source = parseShader("res/shaders/material.shader");
		BasicMaterialShaderID = CreateShader(source.vertexSource, source.fragmentSource);
		glUseProgram(BasicMaterialShaderID);

		unsigned int location = glGetUniformLocation(BasicMaterialShaderID, "u_MVP");
		glUniformMatrix4fv(location, 1, GL_FALSE, &mvp[0][0]);

		BasicMaterial = { BasicMaterialShaderID, 0, 0, 0, 0, 0 };

		return true;
	}

	void NewFrame() {
		// TODO fix this
		glBindFramebuffer(GL_FRAMEBUFFER, 1);
		glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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

	void RequestCursorLock()	{
		glfwSetInputMode(MainTargetWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	void RequestCursorUnlock() {
		glfwSetInputMode(MainTargetWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	void SetRenderViewProjectionMatrix(glm::mat4 ViewProjectionMatrix) {
		RenderViewProjectionMatrix = ViewProjectionMatrix;
	}


	void DrawMeshBasicMaterial(const MeshComponent& mesh, const glm::mat4& modelTransformMatrix) {
		glm::mat4 MVP = RenderViewProjectionMatrix * modelTransformMatrix;
		glUseProgram(BasicMaterialShaderID);
		int location = glGetUniformLocation(BasicMaterialShaderID, "u_MVP"); // location negative if uniform not found
		glUniformMatrix4fv(location, 1, GL_FALSE, &MVP[0][0]);
		VertexArray& meshVertexArray = VertexArrays[mesh.MeshVertexArrayRegistryIndex];
		meshVertexArray.Bind();
		glDrawElements(GL_TRIANGLES, meshVertexArray.IndexCount, GL_UNSIGNED_INT, nullptr);
		meshVertexArray.Unbind();
	}


	void DrawMesh(const MeshComponent& mesh, glm::mat4 modelTransformMatrix) {
		glm::mat4 MVP = RenderViewProjectionMatrix * modelTransformMatrix;
		Material& meshMaterial = GetMaterial(mesh.MaterialIndex);
		glUseProgram(meshMaterial.ShaderID);
		int location = glGetUniformLocation(BasicMaterialShaderID, "u_MVP"); // location negative if uniform not found
		glUniformMatrix4fv(location, 1, GL_FALSE, &MVP[0][0]);
		VertexArray& meshVertexArray = VertexArrays[mesh.MeshVertexArrayRegistryIndex];
		meshVertexArray.Bind();
		glDrawElements(GL_TRIANGLES, meshVertexArray.IndexCount, GL_UNSIGNED_INT, nullptr);
		meshVertexArray.Unbind();
	}

	VertexArray& AddVertexArray() {
		VertexArrays.emplace_back();
		return VertexArrays.back();
	}

	bool AddGeometry(IcePick::Mesh& mesh) {
		if (!mesh.isValid())
			return false;

		float* vertexData;
		size_t vertexStride;
		unsigned int vertexCount, indexCount;
		unsigned int* indexData;

		mesh.GetMeshData(vertexData, vertexStride, vertexCount, indexData, indexCount);

		IcePickRenderer::VertexArrays.emplace_back();
		VertexArray& vertexArray = IcePickRenderer::VertexArrays.back();
		vertexArray.IndexCount = indexCount;
		vertexArray.Bind();

		VertexBuffer vertexBuffer(vertexData, vertexStride * vertexCount);

		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(2);
		layout.Push<float>(3);

		vertexArray.AddBuffer(vertexBuffer, layout);


		IndexBuffer indexBuffer(indexData, indexCount);


		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		return true;
	}
}


void GLCheckErrors(){
	GLenum error = glGetError();
	while (error != GL_NO_ERROR) {
		std::cout << "Error: " << error << std::endl;
		error = glGetError();
	}
}

void GLClearErrors() {
	while (glGetError() != GL_NO_ERROR);
}


Renderer::Renderer(){
	//m_DrawCallsCount = 0;
	//m_ShaderProgram = 0;
}

Renderer::~Renderer(){
	//glDeleteProgram(m_ShaderProgram);
}


bool IcePick::Renderer::AddGeometry(Mesh& mesh){
	if (!mesh.isValid())
		return false;

	float* vertexData;
	size_t vertexStride;
	unsigned int vertexCount, indexCount;
	unsigned int* indexData;

	mesh.GetMeshData(vertexData, vertexStride, vertexCount, indexData, indexCount);

	IcePickRenderer::VertexArrays.emplace_back();
	VertexArray& vertexArray = IcePickRenderer::VertexArrays.back();
	vertexArray.IndexCount = indexCount;
	vertexArray.Bind();

	VertexBuffer vertexBuffer(vertexData, vertexStride * vertexCount);

	VertexBufferLayout layout;
	layout.Push<float>(3);
	layout.Push<float>(2);
	layout.Push<float>(3);

	vertexArray.AddBuffer(vertexBuffer, layout);


	IndexBuffer indexBuffer(indexData, indexCount);


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}
