#include "Serialize.h"
#include "../../src/Utilities/JsonUtils.h"
#include "../../src/LogSystem.h"
#include <fstream>

constexpr unsigned int MATERIAL_BASE_ASSET_VERSION = 1;

void SerializeMaterialBase(std::filesystem::path assetPath, const IcePick::MaterialBase& materialBase, const Graph& shaderGraph, const IcePick::ShaderSource& shaderSource) {
	nlohmann::json json;

	json["version"] = MATERIAL_BASE_ASSET_VERSION;
	json["Id"] = static_cast<uint64_t>(materialBase.Id);
	json["shaderId"] = static_cast<uint64_t>(materialBase.ShaderId);
	json["graphId"] = static_cast<uint64_t>(materialBase.ShaderGraphId);

	nlohmann::json shaderSourceJsonObject;
	shaderSourceJsonObject["vertex"] = shaderSource.VertexShaderSource;
	shaderSourceJsonObject["fragment"] = shaderSource.FragmentShaderSource;
	json["shaderSource"] = shaderSourceJsonObject;

	nlohmann::json shaderGraphJsonObject = nlohmann::json::array();
	for (const auto& node : shaderGraph) {
		nlohmann::json nodeJsonObject;
		nodeJsonObject["Id"] = static_cast<uint64_t>(node->Id);
		nodeJsonObject["type"] = node->GetNodeType();
		nodeJsonObject["isParameter"] = node->nodeIsParameter;

		nlohmann::json inputPinsArrayJsonObject = nlohmann::json::array();
		for (const auto& nodeInputPin : node->InputPins) {
			nlohmann::json inputPinJsonObject;
			inputPinJsonObject["connectedNodeId"] = static_cast<uint64_t>(nodeInputPin.ConnectedNodeId);
			inputPinJsonObject["connectedPinIndex"] = nodeInputPin.ConnectedPinIndex;
			inputPinsArrayJsonObject.push_back(inputPinJsonObject);
		}
		nodeJsonObject["inputPins"] = inputPinsArrayJsonObject;

		nlohmann::json outputPinsArrayJsonObject = nlohmann::json::array();
		for (const auto& nodeOutputPin : node->OutputPins) {
			nlohmann::json outputPinObject;
			nlohmann::json connectedNodeIdsArray = nlohmann::json::array();
			nlohmann::json connectedPinIndicesArray = nlohmann::json::array();
			
			for (unsigned int index = 0; index < nodeOutputPin.ConnectedNodeIds.size(); index++) {
				connectedNodeIdsArray.push_back(static_cast<uint64_t>(nodeOutputPin.ConnectedNodeIds[index]));
				connectedPinIndicesArray.push_back(nodeOutputPin.ConnectedPinIndices[index]);
			}

			outputPinObject["connectedNodeIds"] = connectedNodeIdsArray;
			outputPinObject["connectedPinIndices"] = connectedPinIndicesArray;
			outputPinsArrayJsonObject.push_back(outputPinObject);
		}
		nodeJsonObject["outputPins"] = outputPinsArrayJsonObject;

		shaderGraphJsonObject.push_back(nodeJsonObject);
	}
	json["shaderGraph"] = shaderGraphJsonObject;

	nlohmann::json textureParameters = nlohmann::json::array();
	for (const auto& materialBaseTexture : materialBase.MaterialTextures) {
		nlohmann::json materialBaseTextureJson;
		materialBaseTextureJson["Id"] = static_cast<uint64_t>(materialBaseTexture.Id);
		materialBaseTextureJson["sampler"] = materialBaseTexture.SamplerIdentifier;
		textureParameters.push_back(materialBaseTextureJson);
	}
	json["textureParameters"] = textureParameters;

	std::ofstream outFile(assetPath);
	if (outFile.is_open()) {
		outFile << std::setw(4) << json; // pretty print
	}
	else {
		IP_LOG("Failed to save material base: " + assetPath.string(), IP_ERROR_LOG);
	}
}