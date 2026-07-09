#pragma once
#include "Bone.h"
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include "../Render Systems/SSBO.h"

namespace IcePick {
	struct SkeletonNodeHierarchy {
		int BoneIndex = -1;
		unsigned int BoneLocalTransformIndex = 0;
		//glm::mat4 BoneLocalTransform{ 1.0f };
		std::vector<SkeletonNodeHierarchy> Children;
	};

	class Skeleton {
	public:
		unsigned int AddOrGetBoneId(std::string boneName);
		bool BoneExists(std::string boneName);
		Bone& GetBone(unsigned int boneId);
		void Bake();
		void Use();
		void UploadBoneData();
		SkeletonNodeHierarchy RootBone;
		glm::mat4 InverseGlobalRootTransform{ 1.0f };
		std::vector<glm::mat4> BoneLocalTransforms;
		std::vector<glm::mat4> BoneParentTransforms;

	private:
		bool m_DataBaked = false;
		std::vector<Bone> m_Bones;
		std::vector<glm::mat4> m_BoneTransforms;
		std::unordered_map<std::string, unsigned int> m_BoneNameToIdMap;
		IcePickRenderer::SSBO m_InternalBonesBuffer;
	};
}