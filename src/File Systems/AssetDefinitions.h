#pragma once
#include "..//Scene Systems/Components.h"
#include <vector>

namespace IcePick {
	struct MeshAsset {
		int ID = -1;
		MeshComponent MeshData[MeshRendererComponent::MaxMeshCount];
	};

	struct TextureAsset {
		int ID = -1;

	};

	struct AudioAsset {
		int ID = -1;

	};

	class Mesh {
	public:
		Mesh();
		Mesh(const char* filePath);
		bool isValid();
		~Mesh();
		/*
		* @brief Get Mesh Data
		* @param Pointer reference to vertex data. Size of vertex data in bytes. Pointer reference to index data. Number of indices.
		* @returns Void.
		*/
		//std::vector<Vertex> m_Vertices;
		std::vector<unsigned int> m_Indices;
		void GetMeshData(float*& VertexData, size_t& Stride, unsigned int& VertexCount, unsigned int*& IndexData, unsigned int& IndexCount);
	private:
	};

}