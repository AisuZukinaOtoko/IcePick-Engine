#pragma once
#include "Utilities/Clock.h"

class DopeSheet {
public:
	DopeSheet();
	void OnUpdate(DeltaTime dt);
	void Render();
private:
	bool m_Open = false;
};