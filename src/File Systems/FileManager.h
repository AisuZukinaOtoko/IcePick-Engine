#pragma once
#include "FileDefinitions.h"
#include <vector>

namespace IcePick {

	class FileManager {
	public:
		//void Export(File& File);
		void Import(const char* file);
		
		/*
		@brief Load mesh file.
		@param Path to file
		@returns ID of the mesh.
		*/
		template<typename T>
		unsigned int LoadMesh(const char* filepath) { static_assert(true); }

		/*
		@brief Load mesh FBX file.
		@param Path to file
		@returns ID of the mesh.
		*/
		template<>
		unsigned int LoadMesh<FBX>(const char* filepath);

		/*
		@brief Load mesh OBJ file.
		@param Path to file
		@returns ID of the mesh.
		*/
		template<>
		unsigned int LoadMesh<OBJ>(const char* filepath);

		/*
		* @brief Get a reference to mesh data.
		* @param ID of the mesh to return.
		* @returns Reference to mesh.
		*/
		Mesh& GetMesh(unsigned int meshID);

	private:
		//std::vector<FBX> m_FBXRegistry;
		//std::vector<OBJ> m_OBJRegistry;
		std::vector<Mesh> m_MeshRegistry;
	};
}

inline IcePick::FileManager GlobalFileSystem;