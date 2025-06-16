#pragma once
#include "../Scene Systems/Components.h"
#include <filesystem>
#include <vector>
#include <unordered_map>

namespace IcePick {
	struct Asset {
		unsigned int ID = -1; // index
		bool AssetLoaded = false;
		std::filesystem::path FilePath;
	};

	class AssetLoader {
	public:
		MeshRendererComponent LoadMesh(std::filesystem::path filePath);
		//void LoadTexture(std::filesystem::path filePath);
		//void LoadAudio(std::filesystem::path filePath);
	private:
		std::unordered_map<std::filesystem::path, MeshRendererComponent> m_loadedAssetPathToMeshRenderer;
	};
}