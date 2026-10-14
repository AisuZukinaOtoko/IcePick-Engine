#include "EngineAPI.h"
#include "../Animation Systems/Skeleton.h"
#include "../Render Systems/Renderer.h"
#include "../Render Systems/Vertex.h"
#include "../Scene Systems/Components.h"

namespace IcePick {
	EngineAPI::EngineAPI(std::shared_ptr<EngineLayer> engine) {
		m_Engine = engine;
	}

	unsigned int EngineAPI::GetTexture(std::filesystem::path texturePath, bool isNonLinearSpace) {
		return m_Engine->CreateTexture(texturePath, isNonLinearSpace);
	}

	unsigned int EngineAPI::GetTextureRenderId(UUID textureId) {
		return m_Engine->GetTextureRenderId(textureId);
	}

	UUID EngineAPI::LoadTextureFromAsset(std::filesystem::path assetPath) {
		return m_Engine->LoadTextureFromAsset(assetPath);
	}

	UUID EngineAPI::LoadMaterialBaseFromAsset(std::filesystem::path assetPath) {
		return m_Engine->m_AssetLoader.m_MaterialLoader.NewMaterialBaseFromAsset(assetPath, m_Engine->m_AssetLoader.m_ShaderLoader);
	}

	UUID EngineAPI::LoadMaterialInstanceFromAsset(std::filesystem::path assetPath) {
		return m_Engine->m_AssetLoader.m_MaterialLoader.NewMaterialInstanceFromAsset(assetPath);
	}

	void EngineAPI::SerializeMaterialBase(std::filesystem::path assetPath, const MaterialBase& materialBase) {
		m_Engine->m_AssetLoader.m_MaterialLoader.SaveMaterialBase(assetPath, materialBase);
	}

	void EngineAPI::SerializeMaterialInstance(std::filesystem::path assetPath, const MaterialInstance& materialInstance) {
		m_Engine->m_AssetLoader.m_MaterialLoader.SaveMaterialInstance(assetPath, materialInstance);
	}

	MaterialBase& EngineAPI::GetMaterialBase(UUID materialBaseId) {
		return m_Engine->m_AssetLoader.m_MaterialLoader.GetMaterialBase(materialBaseId);
	}

	MaterialInstance& EngineAPI::GetMaterialInstance(UUID materialInstanceId) {
		return m_Engine->m_AssetLoader.m_MaterialLoader.GetMaterialInstance(materialInstanceId);
	}

	UUID EngineAPI::CreateShaderFromSource(ShaderSource& source) {
		return m_Engine->m_AssetLoader.m_ShaderLoader.CreateShaderProgram(source);
	}

	ShaderProgram& EngineAPI::GetShaderProgram(UUID shaderId) {
		return m_Engine->m_AssetLoader.m_ShaderLoader.GetShaderProgram(shaderId);
	}

	void EngineAPI::UpdateShaderWithSource(UUID shaderId, ShaderSource& source) {
		m_Engine->m_AssetLoader.m_ShaderLoader.ReloadShaderProgram(shaderId, source);
	}

	std::string EngineAPI::LoadShaderSourceFile(std::filesystem::path filepath) {
		return m_Engine->m_AssetLoader.LoadShaderSourceFromFile(filepath);
	}

	const IcePickRenderer::Texture& EngineAPI::GetTexture(UUID textureId) {
		return m_Engine->m_AssetLoader.m_TextureLoader.GetTexture(textureId);
	}

	UUID EngineAPI::RegisterMaterialBase(const MaterialBase& materialBase) {
		return m_Engine->m_AssetLoader.m_MaterialLoader.NewMaterialBaseFromCopy(materialBase);
	}

	UUID EngineAPI::RegisterMaterialInstance(const MaterialInstance& materialInstance) {
		return m_Engine->m_AssetLoader.m_MaterialLoader.NewMaterialInstanceFromCopy(materialInstance);
	}

	void EngineAPI::UpdateMaterialBase(UUID Id, const MaterialBase& materialBase) {
		m_Engine->m_AssetLoader.m_MaterialLoader.UpdateMaterialBase(Id, materialBase);
	}

	void EngineAPI::UpdateMaterialInstance(UUID Id, const MaterialInstance& materialInstance) {
		m_Engine->m_AssetLoader.m_MaterialLoader.UpdateMaterialInstance(Id, materialInstance);
	}

	MeshRendererComponent EngineAPI::LoadMesh(std::filesystem::path assetPath, ImportSettings importSettings) {
		return m_Engine->m_AssetLoader.LoadMesh(assetPath, importSettings);
	}

	void EngineAPI::RenderMesh(MeshRendererComponent& mesh, glm::mat4& modelMatrix, const entt::entity entityId) {
		if (mesh.MeshType == ImportSettings::MeshType::SKELETAL_MESH) {
			IcePickRenderer::SkinnedMeshData& skinnedMeshData = m_Engine->m_AssetLoader.GetSkinnedMeshData(mesh);
			Skeleton& meshSkeleton = m_Engine->m_AssetLoader.GetSkeletonById(skinnedMeshData.SkeletonId);
			m_Engine->m_Animator.CalculateSkeletonTransforms(meshSkeleton);
			meshSkeleton.Use();
		}

		IcePickRenderer::MeshNode& meshData = m_Engine->m_AssetLoader.GetMeshData(mesh);
		m_Engine->RenderMeshNode(meshData, modelMatrix, mesh.MaterialSlots, entityId);
	}

	void EngineAPI::RenderLine(glm::vec3 startPoint, glm::vec3 endPoint, glm::vec4 colour) {
		IcePickRenderer::LinePointVertex3D point1{ startPoint, colour };
		IcePickRenderer::LinePointVertex3D point2{ endPoint, colour };
		IcePickRenderer::DrawLine(point1, point2);
	}

	Skeleton& EngineAPI::GetSkeletonById(UUID skeletonId) {
		return m_Engine->m_AssetLoader.GetSkeletonById(skeletonId);
	}

	void EngineAPI::CreateNewTerrain(unsigned int resolution) {
		m_Engine->m_TerrainSystem.CreateTerrain(resolution, m_Engine->m_AssetLoader.GetTextureLoader());
	}

	bool EngineAPI::IsTerrainCreated() {
		return m_Engine->m_TerrainSystem.IsCreated();
	}

	IcePickRenderer::StaticMeshData& EngineAPI::GetStaticMeshDataById(UUID meshDataId) {
		return m_Engine->m_AssetLoader.m_MeshLoader.GetStaticMeshById(meshDataId);
	}

	IcePickRenderer::SkinnedMeshData& EngineAPI::GetSkinnedMeshDataById(UUID meshDataId) {
		return m_Engine->m_AssetLoader.m_MeshLoader.GetSkinnedMeshById(meshDataId);
	}

	ScriptComponent EngineAPI::LoadScript(std::filesystem::path scriptPath, entt::entity entityId) {
		return m_Engine->m_ScriptRunner.CreateScriptComponentFromFile(scriptPath, entityId);
	}

	void EngineAPI::ReloadScripts() {
		m_Engine->m_ScriptRunner.ReloadScripts();
	}

	std::filesystem::path EngineAPI::GetScriptPathById(UUID scriptId) {
		return m_Engine->m_ScriptRunner.GetScriptPath(scriptId);
	}

	void EngineAPI::SetRenderTargetDefault() {
		m_Engine->SetRenderTargetDefault();
	}

	void EngineAPI::SetRenderTargetFrameBuffer() {
		m_Engine->SetRenderTargetFrameBuffer();
	}

	void EngineAPI::CopyFrameBufferResultToDefaultBuffer() {
		m_Engine->CopyActiveFrameBufferToDefaultFrameBuffer();
	}

	void EngineAPI::ReloadShaders() {
		m_Engine->ReloadShaders();
	}

	SceneRenderDetails& EngineAPI::GetSceneRenderDetails() {
		return m_Engine->GetSceneRenderDetails();
	}

	void EngineAPI::UploadSceneRenderDetails() {
		m_Engine->UploadSceneRenderDetails();
	}

	void EngineAPI::BindSceneRenderDetails(unsigned int bindLocation) {
		m_Engine->m_SceneRenderDetailsUniformBuffer.Bind(bindLocation);
	}

	bool& EngineAPI::QueryEngineRenderDebugPhysics() {
		return m_Engine->m_RenderDebugPhysics;
	}

	void EngineAPI::SetEngineRuntimeState(RuntimeState newRuntimeState) {
		m_Engine->m_CurrentRuntimeState = newRuntimeState;
		switch (newRuntimeState) {
		case RuntimeState::STOPPED:
			m_Engine->OnEndScene();
			break;
		case RuntimeState::RUNNING:
			m_Engine->OnBeginScene();
		}
	}

	RuntimeState EngineAPI::QueryEngineRuntimeState() {
		return m_Engine->m_CurrentRuntimeState;
	}

	void EngineAPI::GetRendererWindowSize(int& x, int& y) {
		glm::ivec2 windowSize = IcePickRenderer::GetRendererWindowSize();
		x = windowSize.x;
		y = windowSize.y;
	}

	glm::vec2 EngineAPI::GetMousePos() {
		return IcePickRenderer::GetMousePos();
	}

	glm::vec2 EngineAPI::GetMouseDelta() {
		return IcePickRenderer::GetMouseDelta();
	}

	void EngineAPI::GetEntityMatPixelData(int x, int y, void* pixelData) {
		m_Engine->GetEntityMatPixelData(x, y, pixelData);
	}

	EngineAPI::~EngineAPI() {

	}
}
