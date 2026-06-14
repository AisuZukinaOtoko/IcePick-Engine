#include "../Render Systems/Renderer.h"
#include "../Render Systems/Vertex.h"
#include "../Render Systems/VertexBuffer.h"
#include "../Render Systems/IndexBuffer.h"
#include "../Render Systems/VertexLayout.h"

#include "../File Systems/TextureLoader.h"
#include "../File Systems/MaterialLoader.h"
#include "MeshLoader.h"
#include "../Scene Systems/Components.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../LogSystem.h"

namespace IcePick {
	MeshLoader::MeshLoader() {

	}

	void MeshLoader::Init() {

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
			aiProcess_OptimizeMeshes
		);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::string errorString = "Assimp Error: " + std::string(importer.GetErrorString());
			IP_LOG(errorString.c_str(), IP_ERROR_LOG);
			return returnMeshRendererComponent;
		}

		textureLoader.SetLoaderBasePath(filePath.parent_path());

		std::vector<UUID> sceneVertexArrays;
		ParseImportMeshData(scene, sceneVertexArrays, importSettings);
		IcePickRenderer::MeshNode sceneRootNode;
		ParseImportNodeTree(scene->mRootNode, sceneRootNode, returnMeshRendererComponent.MaterialSlots, scene, sceneVertexArrays, materialLoader, textureLoader);

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
			skinnedMesh.RootNode = sceneRootNode;
			returnMeshRendererComponent.meshDataId = RegisterSkinnedMesh(skinnedMesh);
			break;
		}
		}

		materialLoader.CleanUpAfterLoad();
		textureLoader.CleanUpAfterLoad();

		return returnMeshRendererComponent;
	}

	void MeshLoader::ParseImportMeshData(const aiScene* scene, std::vector<UUID>& loadVertexArrays, const ImportSettings& importSettings) {
		aiMesh** meshList = scene->mMeshes;
		for (int i = 0; i < scene->mNumMeshes; i++) { // upload each mesh as a vertex array to the GPU

			aiMesh* mesh = meshList[i];
			std::vector<unsigned int> indices;
			std::vector<IcePickRenderer::StaticVertex3D> staticMeshVertices;
			std::vector<IcePickRenderer::SkinnedVertex3D> skinnedMeshVertices;

			for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
				aiVector3D pos = mesh->mVertices[i];
				aiVector3D uv = aiVector3D(0.0f);
				aiVector3D norm = aiVector3D(0.0f);
				float skinnedMeshBoneIds[IcePickRenderer::SkinnedVertex3D::MaxEffectiveBoneCount]{ 0.0f, 0.0f, 0.0f, 0.0f};
				float skinnedMeshBoneWeights[IcePickRenderer::SkinnedVertex3D::MaxEffectiveBoneCount]{ 0.0f, 0.0f, 0.0f, 0.0f };

				if (mesh->HasTextureCoords(0)) {
					uv = mesh->mTextureCoords[0][i];
				}

				if (mesh->HasNormals()) {
					norm = mesh->mNormals[i];
				}

				if (mesh->HasBones()) {
					IP_LOG("Mesh has bones.");
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

			for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
				const aiFace& face = mesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++) {
					indices.push_back(face.mIndices[j]);
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

	void MeshLoader::ParseImportNodeTree(const aiNode* sceneNode, IcePickRenderer::MeshNode& parent, std::vector<UUID>& materialSlots, const aiScene* scene, std::vector<UUID>& sceneVertexArrayIds, MaterialLoader& materialLoader, TextureLoader& textureLoader) {
		IcePickRenderer::MeshNode currentNode;
		if (sceneNode->mNumMeshes > 0) {
			currentNode.VertexArrayIds.reserve(sceneNode->mNumMeshes);
			for (int i = 0; i < sceneNode->mNumMeshes; i++) {
				const unsigned int meshIndex = sceneNode->mMeshes[i];
				UUID nodeMaterialInsatnceId = materialLoader.NewMaterialInstanceFromScene(scene, scene->mMeshes[meshIndex]->mMaterialIndex, textureLoader);
				unsigned int materialSlotIndex = GetMeshMaterialSlot(materialSlots, nodeMaterialInsatnceId);
				currentNode.VertexArrayIds.push_back(sceneVertexArrayIds[meshIndex]);
				currentNode.MaterialSlotIndices.push_back(materialSlotIndex);
			}
		}

		const aiMatrix4x4& t = sceneNode->mTransformation;
		glm::mat4 nodeTransform = {
			t.a1, t.b1, t.c1, t.d1,
			t.a2, t.b2, t.c2, t.d2,
			t.a3, t.b3, t.c3, t.d3,
			t.a4, t.b4, t.c4, t.d4,
		};

		currentNode.NodeTransform = nodeTransform;
		parent.ChildNodes.push_back(currentNode);

		if (sceneNode->mNumChildren == 0)
			return;

		for (int i = 0; i < sceneNode->mNumChildren; i++) {
			ParseImportNodeTree(sceneNode->mChildren[i], parent.ChildNodes.back(), materialSlots, scene, sceneVertexArrayIds, materialLoader, textureLoader);
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