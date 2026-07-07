#include "../Render Systems/Renderer.h"
#include "../Render Systems/Vertex.h"
#include "../Render Systems/VertexBuffer.h"
#include "../Render Systems/IndexBuffer.h"
#include "../Render Systems/VertexLayout.h"

#include "../File Systems/TextureLoader.h"
#include "../File Systems/MaterialLoader.h"
#include "../File Systems/ShaderLoader.h"
#include "MeshLoader.h"
#include "../Scene Systems/Components.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../LogSystem.h"

static glm::mat4 AssimpMatrixToGlmMatrix(const aiMatrix4x4& matrix) {
	glm::mat4 glmMatrix = {
		matrix.a1, matrix.b1, matrix.c1, matrix.d1,
		matrix.a2, matrix.b2, matrix.c2, matrix.d2,
		matrix.a3, matrix.b3, matrix.c3, matrix.d3,
		matrix.a4, matrix.b4, matrix.c4, matrix.d4,
	};
	return glmMatrix;
}

namespace IcePick {
	MeshLoader::MeshLoader() {

	}

	void MeshLoader::Init(ShaderLoader& shaderLoader) {
		//ShaderSource defaultSkinnedShaderSource;
		//defaultSkinnedShaderSource.VertexShaderSource = shaderLoader.LoadFile("Game Engine/res/Shaders/skinning.vert.shader", 0);
		//defaultSkinnedShaderSource.FragmentShaderSource = shaderLoader.LoadFile("Game Engine/res/Shaders/default.frag.shader", 0);

		//m_DefaultSkinnedMeshShaderProgramId = shaderLoader.CreateShaderProgram(defaultSkinnedShaderSource);
		//IP_LOG(std::to_string(m_DefaultSkinnedMeshShaderProgramId));
	}

	UUID MeshLoader::RegisterVertexArray(const IcePickRenderer::VertexArray& vertexArray) {
		UUID newVertexArrayId;
		m_LoadedVertexArrays.insert({ newVertexArrayId, vertexArray });
		return newVertexArrayId;
	}

	UUID MeshLoader::RegisterStaticMesh(const IcePickRenderer::StaticMeshData staticMesh) {
		UUID newStaticMeshId;
		m_LoadedStaticMeshes.insert({ newStaticMeshId, staticMesh });
		return newStaticMeshId;
	}

	UUID MeshLoader::RegisterSkinnedMesh(const IcePickRenderer::SkinnedMeshData skinnedMesh) {
		UUID newSkinnedMeshId;
		m_LoadedSkinnedMeshes.insert({ newSkinnedMeshId, skinnedMesh });
		return newSkinnedMeshId;
	}

	UUID MeshLoader::RegisterMeshSkeleton(const Skeleton& skeleton) {
		UUID newSkeletonId;
		m_LoadedSkeletons.insert({ newSkeletonId, skeleton });
		return newSkeletonId;
	}

	IcePickRenderer::StaticMeshData& MeshLoader::GetStaticMeshById(UUID staticMeshId) {
		if (m_LoadedStaticMeshes.find(staticMeshId) != m_LoadedStaticMeshes.end()) {
			return m_LoadedStaticMeshes[staticMeshId];
		}

		return m_DefaultEmptyStaticMesh;
	}

	IcePickRenderer::SkinnedMeshData& MeshLoader::GetSkinnedMeshById(UUID skinnedMeshId) {
		if (m_LoadedSkinnedMeshes.find(skinnedMeshId) != m_LoadedSkinnedMeshes.end()) {
			return m_LoadedSkinnedMeshes[skinnedMeshId];
		}

		return m_DefaultEmptySkinnedMesh;
	}

	IcePickRenderer::VertexArray& MeshLoader::GetMeshNodeVertexArray(UUID vertexArrayId) {
		if (m_LoadedVertexArrays.find(vertexArrayId) != m_LoadedVertexArrays.end()) {
			return m_LoadedVertexArrays[vertexArrayId];
		}

		return m_DefaultInvalidVertexArray;
	}

	Skeleton& MeshLoader::GetSkeletonById(UUID skeletonId) {
		if (m_LoadedSkeletons.find(skeletonId) != m_LoadedSkeletons.end()) {
			return m_LoadedSkeletons[skeletonId];
		}

		return m_DefaultEmptySkeleton;
	}

	MeshRendererComponent MeshLoader::ImportMesh(std::filesystem::path filePath, MaterialLoader& materialLoader, TextureLoader& textureLoader, const ImportSettings& importSettings) {
		MeshRendererComponent returnMeshRendererComponent;

		if (m_LoadedPathToMeshRenderer.find(filePath) != m_LoadedPathToMeshRenderer.end()) {
			return m_LoadedPathToMeshRenderer[filePath];
		}

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filePath.string(), aiProcess_Triangulate |
			aiProcess_FixInfacingNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_GenUVCoords |
			aiProcess_OptimizeMeshes |
			aiProcess_PopulateArmatureData
		);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::string errorString = "Assimp Error: " + std::string(importer.GetErrorString());
			IP_LOG(errorString.c_str(), IP_ERROR_LOG);
			return returnMeshRendererComponent;
		}

		textureLoader.SetLoaderBasePath(filePath.parent_path());

		std::vector<UUID> sceneVertexArrays;
		Skeleton sceneSkeleton;
		ParseImportMeshData(scene, sceneVertexArrays, sceneSkeleton, importSettings);
		IcePickRenderer::MeshNode sceneRootNode;
		ParseImportNodeTree(scene->mRootNode, sceneRootNode, returnMeshRendererComponent.MaterialSlots, scene, sceneVertexArrays, materialLoader, textureLoader, importSettings);

		if (importSettings.LoadSkeleton) {
			ParseImportSkeletonHierarchy(scene->mRootNode, sceneSkeleton.RootBone, sceneSkeleton);
			sceneSkeleton.InverseGlobalRootTransform = glm::inverse(AssimpMatrixToGlmMatrix(scene->mRootNode->mTransformation));
		}

		switch (importSettings.LoadMeshAs) {
		case ImportSettings::MeshType::STATIC_MESH:
		{
			IcePickRenderer::StaticMeshData staticMesh;
			staticMesh.RootNode = sceneRootNode;
			returnMeshRendererComponent.meshDataId = RegisterStaticMesh(staticMesh);
			break;
		}
		case ImportSettings::MeshType::SKELETAL_MESH:
		{
			IcePickRenderer::SkinnedMeshData skinnedMesh;
			sceneSkeleton.Bake();

			skinnedMesh.RootNode = sceneRootNode;
			skinnedMesh.SkeletonId = RegisterMeshSkeleton(sceneSkeleton);
			returnMeshRendererComponent.meshDataId = RegisterSkinnedMesh(skinnedMesh);
			break;
		}
		}
		returnMeshRendererComponent.MeshType = importSettings.LoadMeshAs;
		returnMeshRendererComponent.MeshCount = scene->mNumMeshes;

		materialLoader.CleanUpAfterLoad();
		textureLoader.CleanUpAfterLoad();

		m_LoadedPathToMeshRenderer.insert({ filePath, returnMeshRendererComponent });
		return returnMeshRendererComponent;
	}

	void MeshLoader::ParseImportMeshData(const aiScene* scene, std::vector<UUID>& loadVertexArrays, Skeleton& loadSkeleton, const ImportSettings& importSettings) {
		aiMesh** meshList = scene->mMeshes;

		for (int m = 0; m < scene->mNumMeshes; m++) { // upload each mesh as a vertex array to the GPU

			aiMesh* mesh = meshList[m];
			std::vector<unsigned int> indices;
			std::vector<IcePickRenderer::StaticVertex3D> staticMeshVertices;
			std::vector<IcePickRenderer::SkinnedVertex3D> skinnedMeshVertices;

			for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
				aiVector3D pos = mesh->mVertices[v];
				aiVector3D uv = aiVector3D(0.0f);
				aiVector3D norm = aiVector3D(0.0f);
				float skinnedMeshBoneIds[IcePickRenderer::SkinnedVertex3D::MaxEffectiveBoneCount]{ 0.0f, 0.0f, 0.0f, 0.0f};
				float skinnedMeshBoneWeights[IcePickRenderer::SkinnedVertex3D::MaxEffectiveBoneCount]{ 0.0f, 0.0f, 0.0f, 0.0f };

				if (mesh->HasTextureCoords(0)) {
					uv = mesh->mTextureCoords[0][v];
				}

				if (mesh->HasNormals()) {
					norm = mesh->mNormals[v];
				}

				glm::vec3 position = glm::vec3(pos.x, pos.y, pos.z);
				glm::vec3 normal = glm::vec3(norm.x, norm.y, norm.z);
				glm::vec2 textureCoords = glm::vec2(uv.x, uv.y);

				if (importSettings.LoadMeshAs == ImportSettings::MeshType::SKELETAL_MESH) {
					skinnedMeshVertices.emplace_back(position, normal, textureCoords);
				}
				else {
					staticMeshVertices.emplace_back(position, normal, textureCoords);
				}
			}

			for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
				const aiFace& face = mesh->mFaces[f];
				for (unsigned int j = 0; j < face.mNumIndices; j++) {
					indices.push_back(face.mIndices[j]);
				}
			}

			if (importSettings.LoadSkeleton) {
				for (unsigned int b = 0; b < mesh->mNumBones; b++) {
					aiBone* bone = mesh->mBones[b];
					unsigned int boneId = loadSkeleton.AddOrGetBoneId(bone->mName.C_Str());
					Bone& skeletonBone = loadSkeleton.GetBone(boneId);

					skeletonBone.OffsetMatrix = AssimpMatrixToGlmMatrix(bone->mOffsetMatrix);

					if (importSettings.LoadMeshAs == ImportSettings::MeshType::SKELETAL_MESH) {
						for (unsigned int w = 0; w < bone->mNumWeights; w++) {
							aiVertexWeight weight =	bone->mWeights[w];
							skinnedMeshVertices[weight.mVertexId].AddBone(boneId, weight.mWeight);
						}
					}
				}				
			}

			IcePickRenderer::VertexArray meshVertexArray;
			meshVertexArray.Init();
			meshVertexArray.IndexCount = mesh->mNumFaces * 3;
			meshVertexArray.Bind();

			void* vertexData = nullptr;
			unsigned int vertexDataSize = 0;
			IcePickRenderer::VertexLayout vertexLayout;

			switch (importSettings.LoadMeshAs) {
			case ImportSettings::MeshType::STATIC_MESH:
				{
					vertexData = staticMeshVertices.data();
					vertexDataSize = sizeof(IcePickRenderer::StaticVertex3D) * staticMeshVertices.size();
					vertexLayout = IcePickRenderer::StaticVertex3D::GetVertexLayout();
					break;
				}
			case ImportSettings::MeshType::SKELETAL_MESH:
				{
					vertexData = skinnedMeshVertices.data();
					vertexDataSize = sizeof(IcePickRenderer::SkinnedVertex3D) * skinnedMeshVertices.size();
					vertexLayout = IcePickRenderer::SkinnedVertex3D::GetVertexLayout();
					break;
				}
			}

			VertexBuffer vertexBuffer(vertexData, vertexDataSize);
			vertexBuffer.Bind();
			meshVertexArray.AddBuffer(vertexBuffer, vertexLayout);

			IndexBuffer indexBuffer(indices.data(), mesh->mNumFaces * 3);
			indexBuffer.Bind();

			meshVertexArray.Unbind();
			indexBuffer.Unbind();
			vertexBuffer.Unbind();

			UUID vertexArrayId = RegisterVertexArray(meshVertexArray);
			loadVertexArrays.push_back(vertexArrayId);
		}
	}

	unsigned int MeshLoader::GetMeshMaterialSlot(std::vector<UUID>& materialSlots, UUID meshMaterial) {
		for (unsigned int i = 0; i < materialSlots.size(); i++) {
			if (materialSlots[i] == meshMaterial)
				return i;
		}

		materialSlots.push_back(meshMaterial);
		return (unsigned int)materialSlots.size() - 1;
	}

	void MeshLoader::ParseImportNodeTree(const aiNode* sceneNode, IcePickRenderer::MeshNode& parent, std::vector<UUID>& materialSlots, const aiScene* scene, std::vector<UUID>& sceneVertexArrayIds, MaterialLoader& materialLoader, TextureLoader& textureLoader, const ImportSettings& importSettings) {
		IcePickRenderer::MeshNode currentNode;
		if (sceneNode->mNumMeshes > 0) {
			currentNode.VertexArrayIds.reserve(sceneNode->mNumMeshes);
			for (int i = 0; i < sceneNode->mNumMeshes; i++) {
				const unsigned int meshIndex = sceneNode->mMeshes[i];
				UUID nodeMaterialInsatnceId = materialLoader.NewMaterialInstanceFromScene(scene, scene->mMeshes[meshIndex]->mMaterialIndex, textureLoader, importSettings);
				unsigned int materialSlotIndex = GetMeshMaterialSlot(materialSlots, nodeMaterialInsatnceId);
				currentNode.VertexArrayIds.push_back(sceneVertexArrayIds[meshIndex]);
				currentNode.MaterialSlotIndices.push_back(materialSlotIndex);
			}
		}

		const aiMatrix4x4& t = sceneNode->mTransformation;
		glm::mat4 nodeTransform = AssimpMatrixToGlmMatrix(sceneNode->mTransformation);

		currentNode.NodeTransform = nodeTransform;
		parent.ChildNodes.push_back(currentNode);

		if (sceneNode->mNumChildren == 0)
			return;

		for (int i = 0; i < sceneNode->mNumChildren; i++) {
			ParseImportNodeTree(sceneNode->mChildren[i], parent.ChildNodes.back(), materialSlots, scene, sceneVertexArrayIds, materialLoader, textureLoader, importSettings);
		}
	}

	void MeshLoader::ParseImportSkeletonHierarchy(const aiNode* sceneNode, SkeletonNodeHierarchy& skeletonNodeHierarchy, Skeleton& skeleton) {
		bool nodeIsBone = skeleton.BoneExists(sceneNode->mName.C_Str());
		if (nodeIsBone) {
			const aiMatrix4x4& t = sceneNode->mTransformation;
			glm::mat4 boneLocalTransform = AssimpMatrixToGlmMatrix(sceneNode->mTransformation);

			unsigned int boneIndex = skeleton.AddOrGetBoneId(sceneNode->mName.C_Str());
			SkeletonNodeHierarchy newSkeletonNode;
			newSkeletonNode.BoneIndex = boneIndex;
			newSkeletonNode.BoneLocalTransform = boneLocalTransform;

			skeletonNodeHierarchy.Children.push_back(newSkeletonNode);
		}

		SkeletonNodeHierarchy& recurseBoneNode = (nodeIsBone) ? skeletonNodeHierarchy.Children.back() : skeletonNodeHierarchy;
		for (int i = 0; i < sceneNode->mNumChildren; i++) {
			ParseImportSkeletonHierarchy(sceneNode->mChildren[i], recurseBoneNode, skeleton);
		}
	}

	MeshRendererComponent MeshLoader::LoadMeshAsset(std::filesystem::path assetPath) {
		return MeshRendererComponent{};
	}

	void MeshLoader::Destroy() {

	}

	MeshLoader::~MeshLoader() {

	}


}