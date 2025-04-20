#include "Materials.h"
#include <vector>

static std::vector<IcePick::Material> MaterialList;

unsigned int IcePick::NewMaterial() {
	MaterialList.emplace_back();
	return MaterialList.size() - 1;
}

IcePick::Material& IcePick::GetMaterial(unsigned int index) {
	return MaterialList[index];
}