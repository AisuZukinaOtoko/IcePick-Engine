#pragma once
#include <memory>
#include "EngineLayer.h"

namespace IcePick {
	class EngineAPI {
	public:
		EngineAPI() = delete;
		EngineAPI(std::shared_ptr<EngineLayer> engine);
		EngineAPI(const EngineAPI& other) = default;
		~EngineAPI();

		unsigned int NewTexture(std::filesystem::path assetPath);

		void SetRenderTargetDefault();
		void SetRenderTargetFrameBuffer();

		void SayHello() {
			
		}
	private:
		std::shared_ptr<EngineLayer> m_Engine;
	};
}