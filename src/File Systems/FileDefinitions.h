#pragma once
#include <vector>

namespace IcePick {
	struct FBX;
	struct OBJ;

	struct Vertex {
		Vertex(float* pos, float* texCrds, float* color) {
			position[0] = pos[0];
			position[1] = pos[1];
			position[2] = pos[2];

			texCoords[0] = texCrds[0];
			texCoords[1] = texCrds[1];

			colourRGB[0] = color[0];
			colourRGB[1] = color[1];
			colourRGB[2] = color[2];
		};
		float position[3];
		float texCoords[2];
		float colourRGB[3];
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
		std::vector<Vertex> m_Vertices;
		std::vector<unsigned int> m_Indices;
		void GetMeshData(float*& VertexData, size_t& Stride, unsigned int& VertexCount, unsigned int*& IndexData, unsigned int& IndexCount);
	private:
	};


	/*class FBX {
	public:
		FBX(const char* filePath);
		~FBX();
		void Serialize();
	private:
		std::vector<Vertex> m_Vertices;
		std::vector<int> m_Indices;
	};

	class OBJ {
	public:
		OBJ(const char* filePath);
		~OBJ();
		void Serialize();
	private:
	};*/
}