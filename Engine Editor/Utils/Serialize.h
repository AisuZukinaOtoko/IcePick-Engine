#pragma once
#include <filesystem>
#include "../../src/Layers/EngineAPI.h"
#include "../Material Editor/Nodes/NodeBase.h"

void SerializeMaterialBase(std::filesystem::path assetPath, const IcePick::MaterialBase& materialBase, const Graph& shaderGraph, const IcePick::ShaderSource& shaderSource);