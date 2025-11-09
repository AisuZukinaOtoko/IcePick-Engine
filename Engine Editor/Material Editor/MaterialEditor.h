#pragma once
#include "../Layers/EngineAPI.h"
#include "../EditorRenderer.h"
#include "../File Systems/MaterialBase.h"
#include "Nodes/NodeBase.h"
#include "NodeGraphManager.h"
#include <memory>
#include <unordered_map>
#include <sstream>
#include <filesystem>

class MaterialEditor {
public:
	MaterialEditor() = delete;
	MaterialEditor(IcePick::EngineAPI engineAPI);
	void SetDropAssetPath(std::filesystem::path filePath);
	void SetEditMaterial(std::filesystem::path materialPath);
	void OnUpdate(DeltaTime dt);
	void Render();
	~MaterialEditor();
private:
	void PreviewMaterial();
	void DrawCanvas();
	void DrawNodes();
	void DrawDragPin();
	void DrawLine(ImVec2 lineStart, ImVec2 lineEnd, bool startIsInputPin);
	void DrawNodeConnections();
	void ShowAddNodeOptions(ImVec2 mousePosInCanvas);
	bool NodeExists(IcePick::UUID nodeId);
	ImVec2 CalculateNodeSize(std::shared_ptr<Node> node);
	ImVec2 CalculateNodePosition(std::shared_ptr<Node> node);
	ImVec2 CalculatePinPosition(std::shared_ptr<Node> node, unsigned int pinIndex, bool isInputPin);
	std::shared_ptr<Node> FindNodeById(IcePick::UUID nodeId);
	void ConnectActivePin(std::shared_ptr<Node> destinationNode, unsigned int destinationPinIndex, bool destinationIsInputPin);

	// Disconnecting an input pin will disconnect the output pin linked to it
	// Disconnecting an output pin will disconnect all input pins linked to it
	void DisconnectPins(std::shared_ptr<Node> node, unsigned int pinIndex, bool isInputPin);

	// Material creation
	bool m_GraphStateChanged = false;
	bool m_AutoCompileGraph = true;
	void CompileMaterial();
	std::string CreateShaderFromGraph(std::stringstream& ss, std::shared_ptr<Node> node, unsigned int outputPinIndex, int recursiveDepth);
	std::unordered_map<IcePick::UUID, std::string, UUIDHasher> m_NodeIdentifiers;
	std::filesystem::path m_DropAssetPath;

	bool m_Open = false;
	IcePick::EngineAPI m_EngineAPI;
	const char* m_ID = "Material Editor";

	std::filesystem::path m_EditMaterialBasePath;
	IcePick::UUID m_EditMaterialBaseId = IcePick::UUID::Unitialised();
	IcePick::UUID m_MaterialEditorMaterialBaseId = IcePick::UUID::Unitialised();
	IcePick::UUID m_MaterialEditorMaterialInstanceId = IcePick::UUID::Unitialised();

	IcePick::MaterialBase m_MaterialEditorMaterialBase;
	IcePick::MaterialInstance m_MaterialEditorMaterialInstance;
	IcePick::ShaderSource m_MaterialEditorShaderSourceTemplate;

	bool m_PinActive = false; //dragging a node pin
	bool m_IsInputPin = false;
	unsigned int m_SourcePinIndex = 0;
	IcePick::UUID m_SourcePinNodeId = IcePick::UUID::Unitialised();
	Graph m_EditMaterialNodeGraph;
	NodeGraphManager m_GraphManager;

	ImVec2 m_CanvasScreenPos;
	ImVec2 m_CanvasScrolling;

	// Preview render variables
	const int previewImageSize = 300;
	float viewRadius = 3.0f;
	float viewAzimuth = 0.0f;
	float viewElevation = 0.0f;
	bool previewWindowRightClicked = false;
	bool lockCursorFirstFrame = false;
	EditorRenderer m_Renderer;
	IcePick::MeshRendererComponent previewMesh;

	// Node render variables
	NodeRenderInfo m_RenderInfo;
};