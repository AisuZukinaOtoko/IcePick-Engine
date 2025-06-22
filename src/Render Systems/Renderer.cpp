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

void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length,
	const GLchar* message, const void* userParam) {
	IP_LOG(message, IP_ERROR_LOG);
	//std::cerr << "[GL DEBUG] " << message << "\n";
}

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
		IP_LOG("Could not open shader file.", IP_ERROR_LOG);
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
		IP_LOG("Failed to compile shader.", IP_ERROR_LOG);
		IP_LOG(err, IP_ERROR_LOG);
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
	static glm::vec3 CameraPosition;
	static glm::mat4 RenderViewProjectionMatrix;
	static glm::mat3 RenderWorldNormalMatrix; // No translation
	static std::vector<VertexArray> VertexArrays;

	static unsigned int BasicMaterialShaderID = 0;
	IcePick::Material BasicMaterial;

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
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glDebugMessageCallback(debugCallback, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
		glDebugMessageControl(GL_DONT_CARE,	GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

		VertexArrays.reserve(30);


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

	void SetRenderCameraWorldPosition(glm::vec3 CameraWorldPosition) {
		CameraPosition = CameraWorldPosition;
	}

	void SetRenderViewProjectionMatrix(glm::mat4 ViewProjectionMatrix) {
		RenderViewProjectionMatrix = ViewProjectionMatrix;
	}

	void SetRenderWorldNormalMatrix(glm::mat3 WorldNormalMatrix) {
		RenderWorldNormalMatrix = WorldNormalMatrix;
	}


	void DrawMeshBasicMaterial(const MeshComponent& mesh, const glm::mat4& modelTransformMatrix) {
		glm::mat4 MVP = RenderViewProjectionMatrix * modelTransformMatrix;
		glUseProgram(BasicMaterialShaderID);

		int location = glGetUniformLocation(BasicMaterialShaderID, "u_MVP"); // location negative if uniform not found
		glUniformMatrix4fv(location, 1, GL_FALSE, &MVP[0][0]);
		location = glGetUniformLocation(BasicMaterialShaderID, "u_Modelmatrix"); // location negative if uniform not found
		glUniformMatrix4fv(location, 1, GL_FALSE, &modelTransformMatrix[0][0]);
		location = glGetUniformLocation(BasicMaterialShaderID, "u_NormalMatrix"); // location negative if uniform not found
		glUniformMatrix3fv(location, 1, GL_FALSE, &RenderWorldNormalMatrix[0][0]);
		location = glGetUniformLocation(BasicMaterialShaderID, "u_CameraPosition"); // location negative if uniform not found
		glUniform3fv(location, 1, &CameraPosition[0]);

		VertexArray& meshVertexArray = VertexArrays[mesh.MeshVertexArrayRegistryIndex];
		meshVertexArray.Bind();
		glDrawElements(GL_TRIANGLES, meshVertexArray.IndexCount, GL_UNSIGNED_INT, nullptr);
		meshVertexArray.Unbind();
	}


	void DrawMesh(const MeshComponent& mesh, glm::mat4 modelTransformMatrix, const IcePick::Material& material) {
		glm::mat4 MVP = RenderViewProjectionMatrix * modelTransformMatrix;
		Material meshMaterial = material;
		//Material& meshMaterial = GetMaterial(mesh.MaterialIndex);
		glUseProgram(meshMaterial.ShaderID);

		int location = glGetUniformLocation(meshMaterial.ShaderID, "u_MVP"); // location negative if uniform not found
		glUniformMatrix4fv(location, 1, GL_FALSE, &MVP[0][0]);
		location = glGetUniformLocation(meshMaterial.ShaderID, "u_Modelmatrix"); // location negative if uniform not found
		glUniformMatrix4fv(location, 1, GL_FALSE, &modelTransformMatrix[0][0]);
		location = glGetUniformLocation(meshMaterial.ShaderID, "u_NormalMatrix"); // location negative if uniform not found
		glUniformMatrix3fv(location, 1, GL_FALSE, &RenderWorldNormalMatrix[0][0]);
		location = glGetUniformLocation(meshMaterial.ShaderID, "u_CameraPosition"); // location negative if uniform not found
		glUniform3fv(location, 1, &CameraPosition[0]);

		// shader textures
		// tell texture units which textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material.AlbedoMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material.NormalMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, material.RoughnessMap);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, material.MetallicMap);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, material.EmissiveMap);

		// tell samplers which texture units
		glUniform1i(glGetUniformLocation(meshMaterial.ShaderID, "MaterialSampleFlags"), material.SampleBitmask);
		glUniform1i(glGetUniformLocation(meshMaterial.ShaderID, "u_AlbedoTexUnit"), 0);
		glUniform1i(glGetUniformLocation(meshMaterial.ShaderID, "u_NormalTexUnit"), 1);
		glUniform1i(glGetUniformLocation(meshMaterial.ShaderID, "u_RoughnessTexUnit"), 2);
		glUniform1i(glGetUniformLocation(meshMaterial.ShaderID, "u_MetallicUnit"), 3);
		glUniform1i(glGetUniformLocation(meshMaterial.ShaderID, "u_EmissiveUnit"), 4);

		glUniform3fv(glGetUniformLocation(meshMaterial.ShaderID, "u_AlbedoColour"), 1, &material.Albedo[0]);
		glUniform3fv(glGetUniformLocation(meshMaterial.ShaderID, "u_SpecularColour"), 1, &material.SpecularColour[0]);

		VertexArray& meshVertexArray = VertexArrays[mesh.MeshVertexArrayRegistryIndex];
		meshVertexArray.Bind();
		glDrawElements(GL_TRIANGLES, meshVertexArray.IndexCount, GL_UNSIGNED_INT, nullptr);
		meshVertexArray.Unbind();
	}

	NewVertexArrayData AddVertexArray() {
		VertexArrays.emplace_back();
		return { VertexArrays.back(), static_cast<unsigned int>(VertexArrays.size() - 1) };
	}
}


void GLCheckErrors(){
	GLenum error = glGetError();
	while (error != GL_NO_ERROR) {
		std::string errorString = "Error: " + std::to_string((int)error);
		IP_LOG(errorString.c_str(), IP_ERROR_LOG);
		std::cout << errorString << std::endl;
		error = glGetError();
	}
}

void GLClearErrors() {
	while (glGetError() != GL_NO_ERROR);
}
