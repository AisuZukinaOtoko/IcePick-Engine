#pragma once
#include "../Layers/EngineAPI.h"
#include "EditorRenderer.h"

class MaterialEditor {
public:
	MaterialEditor() = delete;
	MaterialEditor(IcePick::EngineAPI engineAPI);
	void SetEditMaterial(IcePick::UUID materialID);
	void Render();
	~MaterialEditor();
private:
	void DrawCanvas();
	void DrawNodes();
	bool m_Open = false;
	IcePick::UUID m_EditMaterialId = IcePick::UUID::Unitialised();
	IcePick::EngineAPI m_EngineAPI;
	EditorRenderer m_Renderer;
	const char* m_ID = "Material Editor";

	ImVec2 m_CanvasScrolling;
};