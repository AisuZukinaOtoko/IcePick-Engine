#pragma once
#include "../Layers/EngineAPI.h"

class ThumbnailManager {
public:
	ThumbnailManager() = delete;
	ThumbnailManager(IcePick::EngineAPI engineAPI);

private:
	IcePick::EngineAPI& m_EngineAPI;
};