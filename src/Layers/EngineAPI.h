#pragma once
#include <memory>
#include "EngineLayer.h"
#include "../LogSystem.h"

namespace IcePick {
	class EngineAPI {
	public:
		EngineAPI(std::shared_ptr<EngineLayer> engine);
		EngineAPI(const EngineAPI& other) = default;
		~EngineAPI();

		void SayHello() {
			if (m_Engine) {

			IP_LOG("Hello from the Engine API.");
			}
		}
	private:
		std::shared_ptr<EngineLayer> m_Engine;
	};
}