#include "Serialize.h"
#include "../Material Editor/Nodes/Utils.h"
#include "../Material Editor/Nodes/Nodes.h"
#include "../../src/Utilities/JsonUtils.h"
#include "../../src/LogSystem.h"
#include <cstdlib>
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
		nodeJsonObject["nodeName"] = node->NodeName;
		nodeJsonObject["type"] = node->GetNodeType();
		nodeJsonObject["isParameter"] = node->nodeIsParameter;

		nlohmann::json nodePosition;
		nodePosition["x"] = node->CanvasPosition.x;
		nodePosition["y"] = node->CanvasPosition.y;
		nodeJsonObject["nodePosition"] = nodePosition;

		nlohmann::json inputPinsArrayJsonObject = nlohmann::json::array();
		for (const auto& nodeInputPin : node->InputPins) {
			nlohmann::json inputPinJsonObject;
			inputPinJsonObject["connectedNodeId"] = static_cast<uint64_t>(nodeInputPin.ConnectedNodeId);
			inputPinJsonObject["connectedPinIndex"] = nodeInputPin.ConnectedPinIndex;
			inputPinJsonObject["connectedPinType"] = GetPinTypeString(nodeInputPin.ConnectedPinType);
			inputPinsArrayJsonObject.push_back(inputPinJsonObject);
		}
		nodeJsonObject["inputPins"] = inputPinsArrayJsonObject;

		nlohmann::json outputPinsArrayJsonObject = nlohmann::json::array();
		for (const auto& nodeOutputPin : node->OutputPins) {
			nlohmann::json outputPinObject;
			nlohmann::json connectionDataArray = nlohmann::json::array();

			nlohmann::json connectedNodeIdsArray = nlohmann::json::array();
			nlohmann::json connectedPinIndicesArray = nlohmann::json::array();
			
			for (unsigned int index = 0; index < nodeOutputPin.ConnectedNodeIds.size(); index++) {
				nlohmann::json connectionDataObject;
				connectionDataObject["nodeId"] = static_cast<uint64_t>(nodeOutputPin.ConnectedNodeIds[index]);
				connectionDataObject["pinIndex"] = nodeOutputPin.ConnectedPinIndices[index];
				connectionDataArray.push_back(connectionDataObject);
			}

			outputPinObject["connectionData"] = connectionDataArray;
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
		materialBaseTextureJson["displayName"] = materialBaseTexture.DisplayName;
		textureParameters.push_back(materialBaseTextureJson);
	}
	json["textureParameters"] = textureParameters;

	nlohmann::json floatParameters = nlohmann::json::array();
	for (const auto& materialBaseFloatParameter : materialBase.MaterialFloatParameters) {
		nlohmann::json materialBaseFloatJson;
		materialBaseFloatJson["Id"] = static_cast<uint64_t>(materialBaseFloatParameter.Id);
		materialBaseFloatJson["shaderIdentifier"] = materialBaseFloatParameter.ShaderIdentifier;
		materialBaseFloatJson["displayName"] = materialBaseFloatParameter.DisplayName;
		floatParameters.push_back(materialBaseFloatJson);
	}
	json["floatParameters"] = floatParameters;

	std::ofstream outFile(assetPath);
	if (outFile.is_open()) {
		outFile << std::setw(4) << json; // pretty print
	}
	else {
		IP_LOG("Failed to save material base: " + assetPath.string(), IP_ERROR_LOG);
	}
}

std::shared_ptr<Node> CreateNodeByType(const std::string& nodeType) {
	if (nodeType == "uv") {
		return std::make_shared<UVNode>();
	}
	else if (nodeType == "add") {
		return std::make_shared<AddNode>();
	}
	else if (nodeType == "subtract") {
		return std::make_shared<SubtractNode>();
	}
	else if (nodeType == "multiply") {
		return std::make_shared<MultiplyNode>();
	}
	else if (nodeType == "divide") {
		return std::make_shared<DivideNode>();
	}
	else if (nodeType == "decimal") {
		return std::make_shared<DecimalNode>();
	}
	else if (nodeType == "vec2") {
		return std::make_shared<Vector2Node>();
	}
	else if (nodeType == "vec3") {
		return std::make_shared<Vector3Node>();
	}
	else if (nodeType == "vec4") {
		return std::make_shared<Vector4Node>();
	}
	else if (nodeType == "bsdf") {
		return std::make_shared<BSDFNode>();
	}
	else if (nodeType == "texture") {
		return std::make_shared<TextureNode>(IcePick::UUID::Unitialised());
	}
	else if (nodeType == "float") {
		return std::make_shared<FloatNode>();
	}
	else {
		IP_ASSERT(false, "Node of that type cannot be created from save load.");
	}
}

Graph LoadMaterialBaseEditorData(std::filesystem::path assetPath, IcePick::ShaderSource* loadShaderSource) {
	using nlohmann::json;
	Graph returnMaterialGraph;

	std::ifstream jsonFileStream(assetPath);
	if (jsonFileStream.fail()) {
		IP_LOG("Failed to load material base: " + assetPath.string() + ".", IP_ERROR_LOG);
		return returnMaterialGraph;
	}

	json assetFile = json::parse(jsonFileStream);
	int assetVersion = assetFile.value("version", 0);
	if (assetVersion < MATERIAL_BASE_ASSET_VERSION) {
		IP_LOG("Material base, " + assetPath.string() + ", is an outdated version. Loaded data may be incorrect.", IP_WARN_LOG);
	}

	if (assetFile.contains("shaderGraph") && assetFile["shaderGraph"].is_array()) {
		json& shaderGraphNodes = assetFile["shaderGraph"];

		for (auto nodeIterator = shaderGraphNodes.begin(); nodeIterator != shaderGraphNodes.end(); nodeIterator++) {
			std::string nodeType = nodeIterator->value("type", "base");
			std::shared_ptr<Node> node = CreateNodeByType(nodeType);

			IcePick::UUID nodeId = JsonUtils::GetUint64(*nodeIterator, "Id");
			bool nodeIsParameter = nodeIterator->value("isParameter", false);
			json nodePosition = nodeIterator->at("nodePosition");
			json inputPinsArrayJson = nodeIterator->at("inputPins");
			json outputPinsArrayJson = nodeIterator->at("outputPins");

			node->Id = nodeId;
			node->nodeIsParameter = nodeIsParameter;
			node->CanvasPosition = ImVec2(nodePosition.value("x", 0.0f), nodePosition.value("y", 0.0f));
			node->NodeName = nodeIterator->value("nodeName", "");

			unsigned int inputPinIndex = 0;
			for (auto& inputPin : inputPinsArrayJson) {
				IcePick::UUID connectedNodeId = JsonUtils::GetUint64(inputPin, "connectedNodeId");
				unsigned int connectedPinIndex = inputPin.value("connectedPinIndex", 0);
				std::string connectedPinType = inputPin.value("connectedPinType", "any");

				node->InputPins[inputPinIndex].ConnectedNodeId = connectedNodeId;
				node->InputPins[inputPinIndex].ConnectedPinIndex = connectedPinIndex;
				node->InputPins[inputPinIndex].ConnectedPinType = GetPinTypeFromString(connectedPinType);
				inputPinIndex++;
			}

			unsigned int outputPinIndex = 0;
			for (auto& outputPin : outputPinsArrayJson) {
				json connectedDataArray = outputPin.at("connectionData");
				OutputPin& currentOutputPin = node->OutputPins[outputPinIndex];

				for (auto& connectionDataObject : connectedDataArray) {
					IcePick::UUID connectedNodeId = JsonUtils::GetUint64(connectionDataObject, "nodeId");
					unsigned int connectedPinIndex = connectionDataObject.at("pinIndex");

					currentOutputPin.ConnectedNodeIds.push_back(connectedNodeId);
					currentOutputPin.ConnectedPinIndices.push_back(connectedPinIndex);
				}
				outputPinIndex++;
			}

			returnMaterialGraph.push_back(node);
		}
	}

	jsonFileStream.close();
	return returnMaterialGraph;
}

void CreateNewScriptTemplate(const std::filesystem::path& scriptPath) {
	std::ofstream outFile(scriptPath);
	outFile << 
R"(
-- Delete functions you do not need
function OnCreate(self)
	Log("Hello from the core lua script", 0);
end

function OnUpdate(self, dt)
	
end

function OnDestroy(self)
	Log("Entity destroyed", 0);
end
)";
	outFile.close();
}

void OpenScriptEditor(const std::filesystem::path& scriptPath) {
	std::string editScriptCommand = "code \"" + scriptPath.string() + "\"";
	int result = std::system(editScriptCommand.c_str());
	if (result != 0)
		IP_LOG("Failed to open Visual Studio Code.", IP_ERROR_LOG);
}
