#pragma once
#include <memory>
#include "EngineLayer.h"

namespace IcePick {
	class ShaderProgram;
	struct ShaderSource;
	struct MeshRendererComponent;
	struct ScriptComponent;

	class EngineAPI {
	public:
		EngineAPI() = delete;
		EngineAPI(std::shared_ptr<EngineLayer> engine);
		EngineAPI(const EngineAPI& other) = default;
		~EngineAPI();

		// Textures
		unsigned int GetTexture(std::filesystem::path assetPath);
		unsigned int GetTextureRenderId(UUID textureId);
		UUID LoadTextureFromAsset(std::filesystem::path assetPath);
		const IcePickRenderer::Texture& GetTexture(UUID textureId);

		// Shaders
		UUID CreateShaderFromSource(ShaderSource& source);
		ShaderProgram& GetShaderProgram(UUID shaderId);
		void UpdateShaderWithSource(UUID shaderId, ShaderSource& source);
		std::string LoadShaderSourceFile(std::filesystem::path filepath);

		// Materials
		MaterialBase& GetMaterialBase(UUID materialBaseId);
		MaterialInstance& GetMaterialInstance(UUID materialInstanceId);
		UUID LoadMaterialBaseFromAsset(std::filesystem::path assetPath);
		UUID LoadMaterialInstanceFromAsset(std::filesystem::path assetPath);
		void SerializeMaterialBase(std::filesystem::path assetPath, const MaterialBase& materialBase);
		void SerializeMaterialInstance(std::filesystem::path assetPath, const MaterialInstance& materialInstance);
		UUID RegisterMaterialBase(const MaterialBase& materialBase);
		UUID RegisterMaterialInstance(const MaterialInstance& materialInstance);
		void UpdateMaterialBase(UUID Id, const MaterialBase& materialBase);
		void UpdateMaterialInstance(UUID Id, const MaterialInstance& materialInstance);

		// Meshes
		MeshRendererComponent LoadMesh(std::filesystem::path assetPath, IcePick::ImportSettings importSettings);
		void RenderMesh(MeshRendererComponent& mesh, glm::mat4& modelMatrix, const entt::entity entityId);
		void RenderLine(glm::vec3 startPoint, glm::vec3 endPoint, glm::vec4 colour);

		// Scripts
		ScriptComponent LoadScript(std::filesystem::path scriptPath, entt::entity entityId);
		void ReloadScripts();
		std::filesystem::path GetScriptPathById(UUID scriptId);
		
		// Engine functions
		void SetRenderTargetDefault();
		void SetRenderTargetFrameBuffer();
		void CopyFrameBufferResultToDefaultBuffer();
		void ReloadShaders();

		bool& QueryEngineRenderDebugPhysics();

		void SetEngineRuntimeState(RuntimeState newRuntimeState);
		RuntimeState QueryEngineRuntimeState();

		void GetRendererWindowSize(int& x, int& y);
		glm::vec2 GetMousePos();
		glm::vec2 GetMouseDelta();
		void GetEntityMatPixelData(int x, int y, void* pixelData);
	private:
		std::shared_ptr<EngineLayer> m_Engine;
	};
}