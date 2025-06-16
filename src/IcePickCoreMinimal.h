#pragma once

// Include header for IcePick scenes
#include "Render Systems/Renderer.h"
#include "Scene Systems/Scene.h"
#include "Scene Systems/Entity.h"
#include "Event Systems/EventHandler.h"
#include "Event Systems/Input.h"
#include "LogSystem.h"
#include "File Systems/AssetLoader.h"

namespace IcePick {
	Entity* GetSceneEntity(UUID EntityID);


}