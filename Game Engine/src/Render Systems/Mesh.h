#pragma once
#include "../Scene Systems/UUID.h"
#include <vector>
#include <glm/glm.hpp>

namespace IcePickRenderer {
	struct MeshNode {
		glm::mat4 NodeTransform{ 1.0f };
		std::vector<IcePick::UUID> VertexArrayIds;
		std::vector<unsigned int> MaterialSlotIndices;
		std::vector<MeshNode> ChildNodes;
	};
	
	struct StaticMeshData {
		MeshNode RootNode;
	};

	struct SkinnedMeshData {
		MeshNode RootNode;
	};
}