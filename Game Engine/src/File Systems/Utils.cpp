#include "Utils.h"
#include <algorithm>

std::string SanitizeAssetName(std::string assetName) {
	if (assetName.empty())
		return "Unnamed";

	std::replace(assetName.begin(), assetName.end(), '|', '-');
	return assetName;
}