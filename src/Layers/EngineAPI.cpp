#include "../Render Systems/Renderer.h"
#include "EngineAPI.h"

IcePick::EngineAPI::EngineAPI(std::shared_ptr<EngineLayer> engine) {
	m_Engine = engine;
}

unsigned int IcePick::EngineAPI::GetTexture(std::filesystem::path texturePath) {
	return m_Engine->CreateTexture(texturePath);
}

unsigned int IcePick::EngineAPI::GetTextureRenderId(UUID textureId) {
	return m_Engine->GetTextureRenderId(textureId);
}

IcePick::UUID IcePick::EngineAPI::LoadTextureFromAsset(std::filesystem::path assetPath) {
	return m_Engine->LoadTextureFromAsset(assetPath);
}

IcePick::UUID IcePick::EngineAPI::LoadMaterialBaseFromAsset(std::filesystem::path assetPath) {
	return m_Engine->m_AssetLoader.m_MaterialLoader.NewMaterialBaseFromAsset(assetPath, m_Engine->m_AssetLoader.m_ShaderLoader);
}

IcePick::UUID IcePick::EngineAPI::LoadMaterialInstanceFromAsset(std::filesystem::path assetPath) {
	return m_Engine->m_AssetLoader.m_MaterialLoader.NewMaterialInstanceFromAsset(assetPath);
}

void IcePick::EngineAPI::SerializeMaterialBase(std::filesystem::path assetPath, const MaterialBase& materialBase) {
	m_Engine->m_AssetLoader.m_MaterialLoader.SaveMaterialBase(assetPath, materialBase);
}

void IcePick::EngineAPI::SerializeMaterialInstance(std::filesystem::path assetPath, const MaterialInstance& materialInstance) {
	m_Engine->m_AssetLoader.m_MaterialLoader.SaveMaterialInstance(assetPath, materialInstance);
}

IcePick::MaterialBase& IcePick::EngineAPI::GetMaterialBase(UUID materialBaseId) {
	return m_Engine->m_AssetLoader.m_MaterialLoader.GetMaterialBase(materialBaseId);
}

IcePick::MaterialInstance& IcePick::EngineAPI::GetMaterialInstance(UUID materialInstanceId) {
	return m_Engine->m_AssetLoader.m_MaterialLoader.GetMaterialInstance(materialInstanceId);
}

IcePick::UUID IcePick::EngineAPI::CreateShaderFromSource(ShaderSource& source) {
	return m_Engine->m_AssetLoader.m_ShaderLoader.CreateShaderProgram(source);
}

IcePick::ShaderProgram& IcePick::EngineAPI::GetShaderProgram(UUID shaderId) {
	return m_Engine->m_AssetLoader.m_ShaderLoader.GetShaderProgram(shaderId);
}

void IcePick::EngineAPI::UpdateShaderWithSource(UUID shaderId, ShaderSource& source) {
	m_Engine->m_AssetLoader.m_ShaderLoader.ReloadShaderProgram(shaderId, source);
}

std::string IcePick::EngineAPI::LoadShaderSourceFile(std::filesystem::path filepath) {
	return m_Engine->m_AssetLoader.m_ShaderLoader.LoadFile(filepath, 0);
}

const Texture& IcePick::EngineAPI::GetTexture(UUID textureId) {
	return m_Engine->m_AssetLoader.m_TextureLoader.GetTexture(textureId);
}

IcePick::UUID IcePick::EngineAPI::RegisterMaterialBase(const MaterialBase& materialBase) {
	return m_Engine->m_AssetLoader.m_MaterialLoader.NewMaterialBaseFromCopy(materialBase);
}

IcePick::UUID IcePick::EngineAPI::RegisterMaterialInstance(const MaterialInstance& materialInstance) {
	return m_Engine->m_AssetLoader.m_MaterialLoader.NewMaterialInstanceFromCopy(materialInstance);
}

void IcePick::EngineAPI::UpdateMaterialBase(UUID Id, const MaterialBase& materialBase) {
	m_Engine->m_AssetLoader.m_MaterialLoader.UpdateMaterialBase(Id, materialBase);
}

void IcePick::EngineAPI::UpdateMaterialInstance(UUID Id, const MaterialInstance& materialInstance) {
	m_Engine->m_AssetLoader.m_MaterialLoader.UpdateMaterialInstance(Id, materialInstance);
}

IcePick::MeshRendererComponent IcePick::EngineAPI::LoadMesh(std::filesystem::path assetPath) {
	return m_Engine->m_AssetLoader.LoadMesh(assetPath);
}

void IcePick::EngineAPI::RenderMesh(MeshRendererComponent& mesh, glm::mat4& modelMatrix, const entt::entity entityId) {
	m_Engine->RenderMeshNode(mesh.RootMeshNode, modelMatrix, mesh.MaterialSlots, entityId);
}

IcePick::ScriptComponent IcePick::EngineAPI::LoadScript(std::filesystem::path scriptPath, entt::entity entityId) {
	return m_Engine->m_ScriptRunner.CreateScriptComponentFromFile(scriptPath, entityId);
}

void IcePick::EngineAPI::ReloadScripts() {
	m_Engine->m_ScriptRunner.ReloadScripts();
}

void IcePick::EngineAPI::SetRenderTargetDefault() {
	m_Engine->SetRenderTargetDefault();
}

void IcePick::EngineAPI::SetRenderTargetFrameBuffer() {
	m_Engine->SetRenderTargetFrameBuffer();
}

void IcePick::EngineAPI::ReloadShaders() {
	m_Engine->ReloadShaders();
}

void IcePick::EngineAPI::GetRendererWindowSize(int& x, int& y) {
	glm::ivec2 windowSize = IcePickRenderer::GetRendererWindowSize();
	x = windowSize.x;
	y = windowSize.y;
}

glm::vec2 IcePick::EngineAPI::GetMousePos() {
	return IcePickRenderer::GetMousePos();
}

glm::vec2 IcePick::EngineAPI::GetMouseDelta() {
	return IcePickRenderer::GetMouseDelta();
}

void IcePick::EngineAPI::GetEntityMatPixelData(int x, int y, void* pixelData) {
	m_Engine->GetEntityMatPixelData(x, y, pixelData);
}

IcePick::EngineAPI::~EngineAPI() {

}
