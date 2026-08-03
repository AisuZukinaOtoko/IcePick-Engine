#include "AssetTypes.h"
#include "../Utilities/Assert.h"

namespace IcePick {
	static const char* AssetTypeStrings[AssetTypes::ASSET_TYPE_COUNT];
	static const char* AssetTypeExtensions[AssetTypes::ASSET_TYPE_COUNT];

	void InitialiseAssetTypes() {
		AssetTypeStrings[AssetTypes::STATIC_MESH] = "STATIC_MESH";
		AssetTypeExtensions[AssetTypes::STATIC_MESH] = ".ipstm";

		AssetTypeStrings[AssetTypes::SKELETAL_MESH] = "SKELETAL_MESH";
		AssetTypeExtensions[AssetTypes::SKELETAL_MESH] = ".ipskm";

		AssetTypeStrings[AssetTypes::MATERIAL_BASE] = "MATERIAL_BASE";
		AssetTypeExtensions[AssetTypes::MATERIAL_BASE] = ".ipmtb";

		AssetTypeStrings[AssetTypes::MATERIAL_INSTANCE] = "MATERIAL_INSTANCE";
		AssetTypeExtensions[AssetTypes::MATERIAL_INSTANCE] = ".ipmti";

		AssetTypeStrings[AssetTypes::TEXTURE] = "TEXTURE";
		AssetTypeExtensions[AssetTypes::TEXTURE] = ".iptex";

		AssetTypeStrings[AssetTypes::SKELETON] = "SKELETON";
		AssetTypeExtensions[AssetTypes::SKELETON] = ".ips";

		AssetTypeStrings[AssetTypes::SCRIPT_ASSET] = "SCRIPT";
		AssetTypeExtensions[AssetTypes::SCRIPT_ASSET] = ".lua";

		AssetTypeStrings[AssetTypes::INVALID_ASSET] = "INVALID_ASSET";
		AssetTypeExtensions[AssetTypes::INVALID_ASSET] = ".asset";
	}

	const char* GetAssetTypeString(AssetTypes assetType) {
		IP_ASSERT((assetType >= 0) && (assetType < AssetTypes::ASSET_TYPE_COUNT), "Invalid asset type.");
		return AssetTypeStrings[assetType];
	}

	AssetTypes GetAssetTypeFromExtension(std::string extension) {
		for (unsigned int i = 0; i < AssetTypes::ASSET_TYPE_COUNT; i++) {
			if (extension == AssetTypeExtensions[i])
				return AssetTypeValuesArray[i];
		}
		return AssetTypes::INVALID_ASSET;
	}
}