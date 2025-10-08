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
	bool NodeExists(IcePick::UUID nodeId);
	ImVec2 CalculateNodeSize(std::shared_ptr<Node> node);
	ImVec2 CalculateNodePosition(std::shared_ptr<Node> node);
	ImVec2 CalculatePinPosition(std::shared_ptr<Node> node, unsigned int pinIndex, bool isInputPin);
	std::shared_ptr<Node> FindNodeById(IcePick::UUID nodeId);
	void ConnectActivePin(std::shared_ptr<Node> destinationNode, unsigned int destinationPinIndex, bool destinationIsInputPin);

	// Disconnecting an input pin will disconnect the output pin linked to it
	// Disconnecting an output pin will disconnect all input pins linked to it
	void DisconnectPins(std::shared_ptr<Node> node, unsigned int pinIndex, bool isInputPin);

	bool m_Open = false;
	IcePick::EngineAPI m_EngineAPI;
	const char* m_ID = "Material Editor";

	IcePick::UUID m_EditMaterialId = IcePick::UUID::Unitialised();
	
	bool m_PinActive = false; //dragging a node pin
	bool m_IsInputPin = false;
	unsigned int m_SourcePinIndex = 0;
	IcePick::UUID m_SourcePinNodeId = IcePick::UUID::Unitialised();
	std::vector<std::shared_ptr<Node>> m_EditMaterialNodeGraph;

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
	float nodeWidth = 150.0f;
	float nodePadding = 30.0f;
	float nodeHeaderHeight = 30.0f;
	float nodeCornerRounding = 9.0f;
	float pinYSpacing = 30.0f;
	float pinRadius = 6.0f;
	int pinSegments = 10;
	float lineThickness = 3.0f;
	unsigned int lineSegments = 40;
	ImU32 nodeBgColour = IM_COL32(20, 20, 20, 255);
	ImU32 nodeHeaderColour = IM_COL32(0xD2, 0xA1, 0x02, 255);
	ImU32 nodePinColour = IM_COL32(255, 255, 255, 255);
	ImU32 nodePinColourHovered = IM_COL32(100, 100, 255, 255);
};