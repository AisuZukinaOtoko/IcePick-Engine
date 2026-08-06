#include "AssetTypes.h"
#include "../Utilities/Assert.h"

namespace IcePick {
	static const char* AssetTypeStrings[AssetTypes::ASSET_TYPE_COUNT];
	static const char* AssetTypeExtensions[AssetTypes::ASSET_TYPE_COUNT];

	static constexpr unsigned int NumMeshSourceFileExtensions = 4;
	static const char* MeshSourceFileExtensions[NumMeshSourceFileExtensions] = { ".obj", ".glb", ".gltf", ".fbx" };

	void InitialiseAssetTypes() {
		AssetTypeStrings[AssetTypes::STATIC_MESH] = "STATIC MESH";
		AssetTypeExtensions[AssetTypes::STATIC_MESH] = ".ipstm";

		AssetTypeStrings[AssetTypes::SKELETAL_MESH] = "SKELETAL MESH";
		AssetTypeExtensions[AssetTypes::SKELETAL_MESH] = ".ipskm";

		AssetTypeStrings[AssetTypes::MATERIAL_BASE] = "MATERIAL BASE";
		AssetTypeExtensions[AssetTypes::MATERIAL_BASE] = ".ipmtb";

		AssetTypeStrings[AssetTypes::MATERIAL_INSTANCE] = "MATERIAL INSTANCE";
		AssetTypeExtensions[AssetTypes::MATERIAL_INSTANCE] = ".ipmti";

		AssetTypeStrings[AssetTypes::TEXTURE] = "TEXTURE";
		AssetTypeExtensions[AssetTypes::TEXTURE] = ".iptex";

		AssetTypeStrings[AssetTypes::SKELETON] = "SKELETON";
		AssetTypeExtensions[AssetTypes::SKELETON] = ".ips";

		AssetTypeStrings[AssetTypes::SCRIPT_ASSET] = "SCRIPT";
		AssetTypeExtensions[AssetTypes::SCRIPT_ASSET] = ".lua";

		AssetTypeStrings[AssetTypes::INVALID_ASSET] = "INVALID ASSET";
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

		for (unsigned int i = 0; i < NumMeshSourceFileExtensions; i++) {
			if (extension == MeshSourceFileExtensions[i])
				return AssetTypes::STATIC_MESH;
		}

		return AssetTypes::INVALID_ASSET;
	}
}