#pragma once
#include "UUID.h"
#include <filesystem>
#include <vector>
#include "../File Systems/AssetTypes.h"

namespace IcePick {
	struct AssetRegistryEntry {
		UUID AssetId;
		std::filesystem::path AssetRelativePath;
		AssetTypes AssetType;
	};
	
	class AssetRegistry {
	public:
		AssetRegistry();
		AssetRegistry(const AssetRegistry& other) = delete;
		void Initialise(std::filesystem::path assetRegistryFilePath);
		void RegisterNewAsset(AssetRegistryEntry newAsset);
		void DeleteAsset(UUID assetId);
		void SerializeAssetRegistry();

		const std::vector<AssetRegistryEntry>& GetRegisteredAssets();
	private:
		std::vector<AssetRegistryEntry> m_RegisteredAssets;
		std::filesystem::path m_RegistryFilePath;
		bool m_Initialised = false;
	};

	AssetRegistry& GetAssetRegistry();
}