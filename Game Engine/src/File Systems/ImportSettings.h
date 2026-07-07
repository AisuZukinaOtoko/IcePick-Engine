#pragma once

namespace IcePick {
	struct ImportSettings {
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