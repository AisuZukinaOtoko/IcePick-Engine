#pragma once
#include <filesystem>
#include <memory>
#include "Public/EngineAPI.h"
#include "../Material Editor/Nodes/NodeBase.h"

void SerializeMaterialBase(std::filesystem::path assetPath, const IcePick::MaterialBase& materialBase, const Graph& shaderGraph, const IcePick::ShaderSource& shaderSource);

Graph LoadMaterialBaseEditorData(std::filesystem::path assetPath, IcePick::ShaderSource* loadShaderSource);

std::shared_ptr<Node> CreateNodeByType(const std::string& nodeType);

void CreateNewScriptTemplate(const std::filesystem::path& scriptPath);

void OpenScriptEditor(const std::filesystem::path& scriptPath);