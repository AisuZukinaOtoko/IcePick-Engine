#include "../Render Systems/Renderer.h"
#include "EngineAPI.h"

IcePick::EngineAPI::EngineAPI(std::shared_ptr<EngineLayer> engine) {
	m_Engine = engine;
}

unsigned int IcePick::EngineAPI::NewTexture(std::filesystem::path texturePath) {
	return m_Engine->CreateTexture(texturePath);
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

void IcePick::EngineAPI::GetEntityMatPixelData(int x, int y, void* pixelData) {
	m_Engine->GetEntityMatPixelData(x, y, pixelData);
}

IcePick::EngineAPI::~EngineAPI() {

}
