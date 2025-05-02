#include "FileDefinitions.h"
#include <fstream>
#include <iostream>
#include <string>

IcePick::Mesh::Mesh() {

}

IcePick::Mesh::Mesh(const char* filePath){
	std::ifstream inFile;
	inFile.open(filePath);

	if (inFile.fail()) {
		std::cerr << "Error: Could not open file " << filePath << std::endl;
		return;
	}

	while (!inFile.eof()){
		std::string input;
		inFile >> input;
		int count;

		if (input == "[VERTICES]"){
			float vertexData[3];
			float textureData[2];
			float colourData[3];
			inFile >> count;

			for (int i = 0; i < count; i++) {
				for (int j = 0; j < 3; j++) {
					inFile >> vertexData[j];
				}
				for (int j = 0; j < 2; j++) {
					inFile >> textureData[j];
				}
				for (int j = 0; j < 3; j++) {
					inFile >> colourData[j];
				}
				m_Vertices.push_back(Vertex(vertexData, textureData, colourData));
			}
		}
		if (input == "[INDICES]") {
			int index;
			inFile >> count;

			for (int i = 0; i < count; i++) {
				for (int j = 0; j < 3; j++) {
					inFile >> index;
					m_Indices.push_back(index);
				}
			}
		}
	}

	inFile.close();
}

bool IcePick::Mesh::isValid(){
	unsigned int IndexCount = m_Indices.size();
	return m_Vertices.size() && !(IndexCount % 3) && IndexCount;
}

IcePick::Mesh::~Mesh(){

}

void IcePick::Mesh::GetMeshData(float*& VertexData, size_t& Stride, unsigned int& VertexCount, unsigned int*& IndexData, unsigned int& IndexCount){
	VertexData = &m_Vertices[0].position[0];
	Stride = sizeof(Vertex);
	VertexCount = m_Vertices.size();
	IndexData = &m_Indices[0];
	IndexCount = m_Indices.size();
}