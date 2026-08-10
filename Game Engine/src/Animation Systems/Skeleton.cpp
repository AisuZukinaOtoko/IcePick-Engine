#include "Skeleton.h"

namespace IcePick {
	unsigned int Skeleton::AddOrGetBoneId(std::string boneName) {
		auto iterator = m_BoneNameToIdMap.find(boneName);
		if (iterator != m_BoneNameToIdMap.end())
			return iterator->second;

		if (m_DataBaked) // Cannot add new bone to a baked skeleton. Done to prevent overflowing the statically sized bone SSBO
			return 0;

		unsigned int newBoneId = m_Bones.size();
		m_Bones.push_back({ boneName });
		m_BoneTransforms.emplace_back(1.0f);
		m_BoneNameToIdMap.insert({ boneName, newBoneId });
		return newBoneId;
	}

	Bone& Skeleton::GetBone(unsigned int boneId) {
		return m_Bones[boneId];
	}

	bool Skeleton::BoneExists(std::string boneName) {
		return m_BoneNameToIdMap.find(boneName) != m_BoneNameToIdMap.end();
	}

	void Skeleton::Bake() {
		if (m_DataBaked)
			return;

		m_DataBaked = true;
		m_InternalBonesBuffer.Init(sizeof(glm::mat4) * m_BoneTransforms.size());
		UploadBoneData();
	}

	void Skeleton::Use() {
		unsigned int bindLocation = 0;
		m_InternalBonesBuffer.Bind(bindLocation);
	}

	void Skeleton::UploadBoneData() {
		for (size_t i = 0; i < m_Bones.size(); i++) {
			m_BoneTransforms[i] = m_Bones[i].FinalTransform;
		}

		//for (size_t i = 0; i < Nodes.size(); i++) {
		//	if (Nodes[i].BoneIndex != -1) {
		//		m_BoneTransforms[Nodes[i].BoneIndex] = NodeGlobalTransforms[i];
		//	}
		//}

		Use();
		m_InternalBonesBuffer.UploadData<glm::mat4>(m_BoneTransforms);
	}

	void Skeleton::Destroy() {
		m_Bones.clear();
		m_BoneTransforms.clear();
		m_BoneNameToIdMap.clear();
		m_InternalBonesBuffer.Destroy();

		BoneLocalTransforms.clear();
		BoneParentTransforms.clear();
	}
}