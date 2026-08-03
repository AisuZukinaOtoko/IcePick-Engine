#pragma once
#include "PanelCommon.h"
#include "File Systems/AssetTypes.h"
#include <string>
#include <filesystem>
#include <functional>

class AssetBrowser {
public:
	AssetBrowser() = delete;
	AssetBrowser(IcePick::EngineAPI engineAPI);
	void Init(IcePick::EngineAPI& engineAPI, Styles styles);
	void SetEditMaterialCallback(std::function<void(std::filesystem::path)> callback);
	void Render();
	std::string GetDragFilePath();
	void ChangeBrowsingDirectory(std::filesystem::path directory);
private:
	void DrawProjectFolders();
	void DrawAssets();
	void MaterialBasePopupOptions(const std::filesystem::path& filepath);
	void* GetFileIcon(const std::filesystem::path& extension);

	void HandleMoveAssetToDirectory(std::filesystem::path destinationDirectory);
	void HandleAssetDoubleClick(std::filesystem::path assetPath, IcePick::AssetTypes assetType);

	void DrawProjectFolderRecursive(const std::filesystem::path& currentFolder);
	std::function<void(std::filesystem::path)> EditMaterialCallback;
	Styles m_Styles;
	const char* m_Title = "Asset Browser";

	std::filesystem::path m_DragFilePath;
	IcePick::AssetTypes m_DragAssetType = IcePick::AssetTypes::INVALID_ASSET;

	IcePick::EngineAPI m_EngineAPI;
	std::filesystem::path m_CurrentBrowsingPath;
	std::filesystem::path m_ProjectDirectory;
};