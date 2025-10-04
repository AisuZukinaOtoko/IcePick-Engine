#pragma once
#include "../Layers/EngineAPI.h"
#include "EditorRenderer.h"
#include "Nodes.h"
#include <memory>

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
	void DrawNodeConnections();
	bool NodeExists(IcePick::UUID nodeId);
	std::shared_ptr<Node> FindNodeById(IcePick::UUID nodeId);
	bool m_Open = false;
	IcePick::EngineAPI m_EngineAPI;
	EditorRenderer m_Renderer;
	const char* m_ID = "Material Editor";

	IcePick::UUID m_EditMaterialId = IcePick::UUID::Unitialised();
	//std::vector<Node> m_EditMaterialNodeGraph;
	std::vector<std::shared_ptr<Node>> m_EditMaterialNodeGraph;

	ImVec2 m_CanvasScreenPos;
	ImVec2 m_CanvasScrolling;
};