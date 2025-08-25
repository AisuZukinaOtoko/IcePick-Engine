#include "../Render Systems/Renderer.h"
#include "EngineAPI.h"

IcePick::EngineAPI::EngineAPI(std::shared_ptr<EngineLayer> engine) {
	m_Engine = engine;
}

unsigned int IcePick::EngineAPI::NewTexture(std::filesystem::path texturePath) {
	return m_Engine->CreateTexture(texturePath);
}

IcePick::EngineAPI::~EngineAPI() {

}
