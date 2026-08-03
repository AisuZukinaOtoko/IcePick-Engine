#pragma once
#include <string>

namespace IcePick {
	enum AssetTypes {
		STATIC_MESH = 0,
		SKELETAL_MESH,
		MATERIAL_BASE,
		MATERIAL_INSTANCE,
		TEXTURE,
		SKELETON,
		SCRIPT_ASSET,
		INVALID_ASSET,
		ASSET_TYPE_COUNT
	};

	const AssetTypes AssetTypeValuesArray[ASSET_TYPE_COUNT] = {
		STATIC_MESH,
		SKELETAL_MESH,
		MATERIAL_BASE,
		MATERIAL_INSTANCE,
		TEXTURE,
		SKELETON,
		SCRIPT_ASSET,
		INVALID_ASSET,
	};

	void InitialiseAssetTypes();
	const char* GetAssetTypeString(AssetTypes assetType);
	AssetTypes GetAssetTypeFromExtension(std::string extension);
}