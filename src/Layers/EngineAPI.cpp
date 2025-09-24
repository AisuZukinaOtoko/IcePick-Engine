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
