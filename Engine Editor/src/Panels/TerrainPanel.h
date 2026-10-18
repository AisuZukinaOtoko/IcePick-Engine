#pragma once
#include "../Styles/Styles.h"

namespace IcePick {
	class EngineAPI;
}

class TerrainPanel {
public:
	TerrainPanel();

	void Render(IcePick::EngineAPI engineAPI, const Styles& styles);

	~TerrainPanel();
private:
	void NoTerrainCreatedUI(IcePick::EngineAPI engineAPI);
	void TerrainCreatedUI(IcePick::EngineAPI engineAPI, const Styles& styles);


	unsigned int m_TerrainSize = 2;
	unsigned int m_UnitsPerMetre = 1;
	bool m_InputtingTerrainSettings = false;
};