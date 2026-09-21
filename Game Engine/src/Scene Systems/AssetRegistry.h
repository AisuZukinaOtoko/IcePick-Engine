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
		void Initialise(std::filesystem::path projectRootDirectory);
		bool IsInitialised() { return m_Initialised; }

		void RegisterNewAsset(AssetRegistryEntry newAsset);
		void DeleteAsset(UUID assetId);
		void SerializeAssetRegistry();

		// Asset path is the file path relative to the project path
		std::filesystem::path ResolveAbsolutePathFromAssetPath(std::filesystem::path assetPath);
		std::filesystem::path ResolveAssetPathFromAbsolutePath(std::filesystem::path absolutePath);

		std::filesystem::path GetAssetPathFromAssetId(UUID assetId);
		UUID GetAssetIdFromAssetPath(std::filesystem::path assetPath);

		std::filesystem::path GetProjectRootDirectory();

		const std::vector<AssetRegistryEntry>& GetRegisteredAssets();
	private:
		std::vector<AssetRegistryEntry> m_RegisteredAssets;
		std::filesystem::path m_RegistryFilePath;
		std::filesystem::path m_ProjectRootDirectory;
		bool m_Initialised = false;
	};

	AssetRegistry& GetAssetRegistry();
}