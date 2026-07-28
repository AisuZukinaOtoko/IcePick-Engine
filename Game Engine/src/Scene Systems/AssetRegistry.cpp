#include "AssetRegistry.h"
#include <fstream>
#include "../Utilities/JsonUtils.h"
#include "../LogSystem.h"

constexpr unsigned int ASSET_REGISTRY_VERSION = 1;

namespace IcePick {

	static AssetRegistry s_AssetRegistry;

	AssetRegistry::AssetRegistry() {
		m_RegisteredAssets.push_back({});
		m_RegisteredAssets.push_back({});
		m_RegisteredAssets.push_back({});
		m_RegisteredAssets.push_back({});
		m_RegisteredAssets.push_back({});
		m_RegisteredAssets.push_back({});
	}

	void AssetRegistry::Initialise(std::filesystem::path assetRegistryFilePath) {
		std::ifstream inFile(assetRegistryFilePath);

		if (!inFile.is_open()) {
			IP_LOG("Failed to load asset registry. Could not open file: " + assetRegistryFilePath.string(), IP_ERROR_LOG);
			return;
		}

		m_RegisteredAssets.clear();
		m_RegistryFilePath = assetRegistryFilePath;

		nlohmann::json loadFile = nlohmann::json::parse(inFile);
		unsigned int registryVersion = loadFile.value("version", 0);

		if (registryVersion < ASSET_REGISTRY_VERSION) {
			IP_LOG("Asset registry, " + assetRegistryFilePath.string() + ", is an outdated version. Loaded data may be incorrect.", IP_WARN_LOG);
		}

		if (loadFile.contains("assets") && loadFile["assets"].is_array()) {
			nlohmann::json& registeredAssets = loadFile["assets"];

			for (auto assetIterator = registeredAssets.begin(); assetIterator != registeredAssets.end(); assetIterator++) {
				AssetRegistryEntry loadAsset;
				loadAsset.AssetId = JsonUtils::GetUint64(*assetIterator, "assetId", UUID::Unitialised());
				loadAsset.AssetRelativePath = assetIterator->value("relativePath", "");
				loadAsset.AssetType = static_cast<AssetTypes>(assetIterator->value("assetType", AssetTypes::INVALID_ASSET));
				m_RegisteredAssets.push_back(loadAsset);
			}
		}
	}

	const std::vector<AssetRegistryEntry>& AssetRegistry::GetRegisteredAssets() {
		return m_RegisteredAssets;
	}

	void AssetRegistry::RegisterNewAsset(AssetRegistryEntry newAsset) {
		m_RegisteredAssets.push_back(newAsset);
	}

	void AssetRegistry::DeleteAsset(UUID assetId) {
		for (size_t currentIndex = 0; currentIndex < m_RegisteredAssets.size(); currentIndex++) {
			if (m_RegisteredAssets[currentIndex].AssetId == assetId) {
				size_t lastIndex = m_RegisteredAssets.size() - 1;
				m_RegisteredAssets[currentIndex] = m_RegisteredAssets[lastIndex];
				m_RegisteredAssets.pop_back();
				break;
			}
		}
	}

	void AssetRegistry::SerializeAssetRegistry() {
		nlohmann::json json;
		std::ofstream outFile(m_RegistryFilePath);

		if (!outFile.is_open()) {
			IP_LOG("Failed to save asset registry. Could not open file: " + m_RegistryFilePath.string(), IP_ERROR_LOG);
			return;
		}

		json["version"] = ASSET_REGISTRY_VERSION;
		nlohmann::json registeredAssets = nlohmann::json::array();
		for (size_t assetIndex = 0; assetIndex < m_RegisteredAssets.size(); assetIndex) {
			AssetRegistryEntry& registryEntry = m_RegisteredAssets[assetIndex];
			nlohmann::json registryEntryObject;

			registryEntryObject["assetId"] = static_cast<uint64_t>(registryEntry.AssetId);
			registryEntryObject["relativePath"] = registryEntry.AssetRelativePath;
			registryEntryObject["assetType"] = static_cast<unsigned int>(registryEntry.AssetType);

			registeredAssets.push_back(registryEntryObject);
		}
		json["assets"] = registeredAssets;

		outFile << std::setw(4) << json; //pretty print
	}

	AssetRegistry& GetAssetRegistry() {
		return s_AssetRegistry;
	}

}