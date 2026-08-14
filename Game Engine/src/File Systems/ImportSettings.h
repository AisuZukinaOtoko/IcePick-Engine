#pragma once
#include <string>
#include <filesystem>

namespace IcePick {
	struct ImportSettings {
		std::string ImportAssetName;
		std::filesystem::path ImportTargetLocation;
		enum class MeshType {
			STATIC_MESH = 0,
			SKELETAL_MESH
		} LoadMeshAs{ MeshType::STATIC_MESH };
		bool LoadMesh = true;
		bool LoadMaterials = true;
		bool LoadSkeleton = false;
		bool LoadAnimations = false;
	};
}