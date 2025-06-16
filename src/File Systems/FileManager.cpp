#include "FileManager.h"
#include <fstream>
#include <assert.h>

//void IcePick::FileManager::Export(File& file){
//	file.Serialize();
//}

void IcePick::FileManager::Import(const char* filepath){

}

IcePick::Mesh& IcePick::FileManager::GetMesh(unsigned int meshID){
	assert(meshID < m_MeshRegistry.size());
	return m_MeshRegistry[meshID];
}

//template<>
//unsigned int IcePick::FileManager::LoadMesh<IcePick::FBX>(const char* filepath) {
//	m_MeshRegistry.emplace_back(filepath);
//	return m_MeshRegistry.size() - 1;
//}
//
//template<>
//unsigned int IcePick::FileManager::LoadMesh<IcePick::OBJ>(const char* filepath) {
//	//OBJ file(filepath);
//	//file.Serialize();
//	return -1;
//}
