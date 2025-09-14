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

IcePick::EngineAPI::~EngineAPI() {

}
