#pragma once
#include <string>

namespace IcePick {
	struct ImportSettings {
		std::string ImportName;
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