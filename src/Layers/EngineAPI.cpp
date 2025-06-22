#include "../Render Systems/Renderer.h"
#include "EngineAPI.h"

IcePick::EngineAPI::EngineAPI(std::shared_ptr<EngineLayer> engine) {
	m_Engine = engine;
	IP_LOG("The API has a reference to the engine.");
}

IcePick::EngineAPI::~EngineAPI() {

}
