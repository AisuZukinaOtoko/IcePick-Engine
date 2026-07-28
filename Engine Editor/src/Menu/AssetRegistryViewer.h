#pragma once

class AssetRegistryViewer {
public:
	void Render();
	void SetOpen(bool openState);
private:
	bool m_IsOpen = false;
};