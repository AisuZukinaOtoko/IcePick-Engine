#pragma once
#include "File Systems/ImportSettings.h"

class MeshImportPopup {
public:
	void OpenPopup();
	void Render();
	bool ImportSubmitted() { return m_ImportSubmitted; }
	void HandleSubmit() { m_ImportSubmitted = false; }
	IcePick::ImportSettings GetImportSettings() { return m_ImportSettings;  }
private:
	void ClosePopup();
	bool m_Open = false;
	bool m_ImportSubmitted = false;
	const char* m_Title = "Mesh Import Settings";
	IcePick::ImportSettings m_ImportSettings;
};