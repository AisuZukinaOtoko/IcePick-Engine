#pragma once
#include "Bone.h"
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "../Render Systems/SSBO.h"
#include <string>

namespace IcePick {
	//struct SkeletonNodeHierarchy {
	//	int BoneIndex = -1;
	//	unsigned int BoneLocalTransformIndex = 0;
	//	std::string NodeName;
	//	std::vector<SkeletonNodeHierarchy> Children;
	//};

	struct SkeletonNodeHierarchy {
		int BoneIndex = -1;
		unsigned int BoneLocalTransformIndex = 0;

		int NodeIndex = -1;
		std::string NodeName;
		std::vector<SkeletonNodeHierarchy> Children;
	};

	struct SkeletonNode {
		std::string NodeName;
		glm::mat4 LocalTransform;
		int BoneIndex = -1;
		int ParentNodeIndex = -1;
	};

	class Skeleton {
	public:
		unsigned int AddOrGetBoneId(std::string boneName);
		bool BoneExists(std::string boneName);
		Bone& GetBone(unsigned int boneId);
		void Bake();
		void Use();
		void UploadBoneData();
		void Destroy();
		SkeletonNodeHierarchy RootBone;
		glm::mat4 InverseGlobalRootTransform{ 1.0f };
		std::vector<glm::mat4> BoneLocalTransforms;
		std::vector<glm::mat4> BoneParentTransforms;

		std::vector<SkeletonNode> Nodes;
		std::vector<glm::mat4> NodeGlobalTransforms;
	private:
		bool m_DataBaked = false;
		std::vector<Bone> m_Bones;
		std::vector<glm::mat4> m_BoneTransforms;
		std::unordered_map<std::string, unsigned int> m_BoneNameToIdMap;
		IcePickRenderer::SSBO m_InternalBonesBuffer;
	};
}