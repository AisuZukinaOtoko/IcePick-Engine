#include "MaterialEditor.h"
#include "imgui-docking/imgui.h"
#include "../LogSystem.h"
#include "../Utilities/Assert.h"

MaterialEditor::MaterialEditor(IcePick::EngineAPI engineAPI) :
	m_EngineAPI(engineAPI),
	m_Renderer(engineAPI)
{
    Pin::InitPins();
    m_CanvasScrolling = ImVec2(0.0f, 0.0f);
    m_Renderer.editorCamera.aspectRatio = 1.0f;
    m_Renderer.Init(previewImageSize, previewImageSize);
    previewMesh = m_EngineAPI.LoadMesh("res/Assets/sphere.glb");
}

void MaterialEditor::SetEditMaterial(IcePick::UUID materialID) {
	m_Open = true;
	m_EditMaterialId = materialID;

    m_EditMaterialNodeGraph.clear();
    m_EditMaterialNodeGraph.push_back(std::make_shared<BSDFNode>());
}

void MaterialEditor::OnUpdate(DeltaTime dt) {
    glm::vec2 mouseDelta = (previewWindowRightClicked) ? m_EngineAPI.GetMouseDelta() : glm::vec2(0.0f, 0.0f);
    if (lockCursorFirstFrame) {
        mouseDelta = glm::vec2(0.0f, 0.0f);
        lockCursorFirstFrame = false;
    }
    float mouseSensitivity = 0.01f;

    viewAzimuth -= mouseDelta.x * mouseSensitivity;
    viewElevation += mouseDelta.y * mouseSensitivity;
    viewElevation = glm::clamp(viewElevation, -1.5f, 1.5f);

    glm::vec3& position = m_Renderer.editorCamera.cameraPosition;
    position.x = viewRadius * cos(viewElevation) * sin(viewAzimuth);
    position.y = viewRadius * sin(viewElevation);
    position.z = viewRadius * cos(viewElevation) * cos(viewAzimuth);

    glm::vec3 cameraFront = glm::vec3(0.0f) - position;
    m_Renderer.editorCamera.cameraFront = glm::normalize(cameraFront);
}

void MaterialEditor::Render() {
	if (!m_Open) {
		return;
	}

	ImGui::Begin(m_ID, &m_Open, ImGuiWindowFlags_NoCollapse);

	if (ImGui::BeginTable("Material", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableNextRow(ImGuiTableRowFlags_None);
		ImGui::TableNextColumn();

        PreviewMaterial();
		ImGui::ImageButton("##MaterialButton", (void*)m_Renderer.GetRenderTexture(), ImVec2(previewImageSize, previewImageSize), ImVec2(0, 1), ImVec2(1, 0));
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsItemHovered()) {
            IcePickRenderer::RequestCursorLock();
            previewWindowRightClicked = true;
            lockCursorFirstFrame = true;
        }
        else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            IcePickRenderer::RequestCursorUnlock();
            previewWindowRightClicked = false;
        }
        
        if (ImGui::Button("Compile graph")) {
            CompileMaterial();
        }

		ImGui::TableNextColumn();
		DrawCanvas();
		ImGui::EndTable();
	}
	ImGui::End();
}

void MaterialEditor::PreviewMaterial() {
    m_Renderer.Clear();
    previewMesh.MaterialSlots[0] = m_EditMaterialId;
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    m_Renderer.RenderMesh(previewMesh, modelMatrix);
}

void MaterialEditor::DrawCanvas() {

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));      // Disable padding
	ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 50, 255));  // Set a background color
	ImGui::BeginChild("canvas", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_NoMove);

    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
    m_CanvasScreenPos = canvas_p0;
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
    if (canvas_sz.x < 50.0f)
        canvas_sz.x = 50.0f;
    if (canvas_sz.y < 50.0f)
        canvas_sz.y = 50.0f;
    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

    // Draw background color
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));

    // This will catch our interactions
    ImGui::SetNextItemAllowOverlap();
    ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonRight);
    const bool is_hovered = ImGui::IsItemHovered(); // Hovered
    const bool is_active = ImGui::IsItemActive();   // Held
    const ImVec2 origin(canvas_p0.x + m_CanvasScrolling.x, canvas_p0.y + m_CanvasScrolling.y); // Lock scrolled origin
    const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);


    // Pan (we use a zero mouse threshold when there's no context menu)
    // You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
    if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
        m_CanvasScrolling.x += io.MouseDelta.x;
        m_CanvasScrolling.y += io.MouseDelta.y;
    }

    // Context menu (under default mouse threshold)
    ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
    if (drag_delta.x == 0.0f && drag_delta.y == 0.0f)
        ImGui::OpenPopupOnItemClick("context", ImGuiPopupFlags_MouseButtonRight);
    if (ImGui::BeginPopup("context")) {        
        if (ImGui::MenuItem("Texture Node", NULL, false)) {
            std::shared_ptr<TextureNode> newNode = std::make_shared<TextureNode>(IcePick::UUID::Unitialised());
            newNode->CanvasPosition = mouse_pos_in_canvas;
            m_EditMaterialNodeGraph.push_back(newNode);
        }
        if (ImGui::MenuItem("Vector3 Node", NULL, false)) { 
            std::shared_ptr<Vector3Node> newNode = std::make_shared<Vector3Node>();
            newNode->CanvasPosition = mouse_pos_in_canvas;
            m_EditMaterialNodeGraph.push_back(newNode);
        }
        ImGui::EndPopup();
    }

    // Draw grid + all lines in the canvas
    draw_list->PushClipRect(canvas_p0, canvas_p1, true);
    const float GRID_STEP = 64.0f;
    for (float x = fmodf(m_CanvasScrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
        draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
    for (float y = fmodf(m_CanvasScrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
        draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));

    DrawNodeConnections();
    DrawNodes();

    draw_list->PopClipRect();

	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
	ImGui::EndChild();
}

void MaterialEditor::DrawNodes() {
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 offset = m_CanvasScreenPos;
    ImVec2 mousePos = ImGui::GetMousePos();
    bool mousePressed = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
    bool mouseReleased = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
    float labelPadding = 5.0f;
    ImU32 labelColour = IM_COL32(255, 255, 255, 255);

    for (int i = 0; i < m_EditMaterialNodeGraph.size(); i++) {
        ImGui::PushID(i);
        std::shared_ptr<Node> node = m_EditMaterialNodeGraph[i];

        ImVec2 nodePosition = CalculateNodePosition(node);
        ImVec2 nodeSize = CalculateNodeSize(node);

        draw_list->AddRectFilled(nodePosition, ImVec2(nodePosition.x + nodeSize.x, nodePosition.y + nodeSize.y), nodeBgColour, nodeCornerRounding, ImDrawFlags_RoundCornersAll);
        draw_list->AddRectFilled(nodePosition, ImVec2(nodePosition.x + nodeSize.x, nodePosition.y + nodeHeaderHeight), nodeHeaderColour, nodeCornerRounding, ImDrawFlags_RoundCornersTop);

        ImGui::SetCursorScreenPos(nodePosition);
        ImGui::InvisibleButton("nodeHeader", ImVec2(nodeSize.x, nodeHeaderHeight), ImGuiButtonFlags_MouseButtonLeft);
        const bool nodeHeaderHovered = ImGui::IsItemHovered();
        const bool nodeHeaderHeld = ImGui::IsItemActive();

        for (int j = 0; j < node->InputPins.size(); j++) {
            ImGui::PushID(j);
            ImVec2 pinPosition = CalculatePinPosition(node, j, true);
            float dx = pinPosition.x - mousePos.x;
            float dy = pinPosition.y - mousePos.y;
            bool currentPinHovered = (dx * dx + dy * dy) < (pinRadius * pinRadius);

            ImU32 colour = (currentPinHovered) ? nodePinColourHovered : nodePinColour;
            draw_list->AddCircleFilled(pinPosition, pinRadius, colour, pinSegments);
            ImGui::SetCursorScreenPos(ImVec2(pinPosition.x - pinRadius, pinPosition.y - pinRadius));
            ImGui::InvisibleButton("inputNodePin", ImVec2(pinRadius * 2, pinRadius * 2), ImGuiButtonFlags_MouseButtonLeft);
            bool currentPinActive = ImGui::IsItemActive();

            std::string& label = node->InputPins[j].Label;
            ImVec2 labelSize = ImGui::CalcTextSize(label.c_str());
            ImVec2 labelPosition = ImVec2(pinPosition.x + pinRadius + labelPadding, pinPosition.y - (labelSize.y / 2.0f));
            draw_list->AddText(labelPosition, labelColour, label.c_str());

            if (currentPinActive) {
                m_PinActive = true;
                m_SourcePinIndex = j;
                m_IsInputPin = true;
                m_SourcePinNodeId = node->Id;
                DrawDragPin();
            }

            if (currentPinHovered && mouseReleased && m_PinActive) {
                ConnectActivePin(node, j, true);
            }

            if (ImGui::IsKeyDown(ImGuiKey_LeftShift) && currentPinHovered && mousePressed) {
                DisconnectPins(node, j, true);
            }

            ImGui::PopID();
        }

        for (int j = 0; j < node->OutputPins.size(); j++) {
            ImGui::PushID(j);
            ImVec2 pinPosition = CalculatePinPosition(node, j, false);
            float dx = pinPosition.x - mousePos.x;
            float dy = pinPosition.y - mousePos.y;
            bool currentPinHovered = (dx * dx + dy * dy) < (pinRadius * pinRadius);

            ImU32 colour = (currentPinHovered) ? nodePinColourHovered : nodePinColour;
            draw_list->AddCircleFilled(pinPosition, pinRadius, colour, pinSegments);
            ImGui::SetCursorScreenPos(ImVec2(pinPosition.x - pinRadius, pinPosition.y - pinRadius));
            ImGui::InvisibleButton("outputNodePin", ImVec2(pinRadius * 2, pinRadius * 2), ImGuiButtonFlags_MouseButtonLeft);
            bool currentPinActive = ImGui::IsItemActive();

            std::string& label = node->OutputPins[j].Label;
            ImVec2 labelSize = ImGui::CalcTextSize(label.c_str());          
            ImVec2 labelPosition = ImVec2(pinPosition.x - pinRadius - labelPadding - labelSize.x, pinPosition.y - (labelSize.y / 2.0f));
            draw_list->AddText(labelPosition, IM_COL32(255, 255, 255, 255), label.c_str());

            if (currentPinActive) {
                m_PinActive = true;
                m_SourcePinIndex = j;
                m_IsInputPin = false;
                m_SourcePinNodeId = node->Id;
                DrawDragPin();
            }

            if (currentPinHovered && mouseReleased && m_PinActive) {
                ConnectActivePin(node, j, false);
            }

            if (ImGui::IsKeyDown(ImGuiKey_LeftShift) && currentPinHovered && mousePressed) {
                DisconnectPins(node, j, false);
            }

            ImGui::PopID();
        }

        // Only update node position at the end to avoid inconsistencies with pins
        if (nodeHeaderHeld && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            node->CanvasPosition.x += io.MouseDelta.x;
            node->CanvasPosition.y += io.MouseDelta.y;
        }

        ImGui::PopID();
    }

    if (mouseReleased)
        m_PinActive = false;
}

void MaterialEditor::DrawDragPin() {
    if (!m_PinActive)
        return;

    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    std::shared_ptr<Node> node = FindNodeById(m_SourcePinNodeId);
    ImVec2 lineP1 = CalculatePinPosition(node, m_SourcePinIndex, m_IsInputPin);
    ImVec2 lineP4 = ImGui::GetMousePos();

    DrawLine(lineP1, lineP4, m_IsInputPin);
}

void MaterialEditor::DrawLine(ImVec2 lineStart, ImVec2 lineEnd, bool startIsInputPin) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    int sign = (startIsInputPin) ? -1 : 1;
    float innerPointXOffset = std::abs(lineEnd.x - lineStart.x) / 3.0f;
    float innerPointYOffset = (lineEnd.y - lineStart.y) / 8.0f;

    ImVec2 lineP2 = ImVec2(lineStart.x + innerPointXOffset * sign, lineStart.y + innerPointYOffset);
    ImVec2 lineP3 = ImVec2(lineEnd.x - innerPointXOffset * sign, lineEnd.y - innerPointYOffset);

    draw_list->AddBezierCubic(lineStart, lineP2, lineP3, lineEnd, nodePinColour, lineThickness, lineSegments);
}

void MaterialEditor::DrawNodeConnections() {
    for (std::shared_ptr<Node> node : m_EditMaterialNodeGraph) {
        for (int i = 0; i < node->InputPins.size(); i++) {
            InputPin& nodeInputPin = node->InputPins[i];
            if (nodeInputPin.ConnectedNodeId == IcePick::UUID::Unitialised()) // not connected to any node
                continue;

            std::shared_ptr<Node> connectedOutputNode = FindNodeById(nodeInputPin.ConnectedNodeId);
            unsigned int connectedOutputPinIndex = nodeInputPin.ConnectedPinIndex;

            ImVec2 inputPinLocation = CalculatePinPosition(node, i, true);
            ImVec2 outputPinLocation = CalculatePinPosition(connectedOutputNode, connectedOutputPinIndex, false);

            DrawLine(inputPinLocation, outputPinLocation, true);
        }
    }    
}

bool MaterialEditor::NodeExists(IcePick::UUID nodeId) {
    for (const std::shared_ptr<Node> node : m_EditMaterialNodeGraph) {
        if (node->Id == nodeId)
            return true;
    }
    return false;
}

ImVec2 MaterialEditor::CalculateNodeSize(std::shared_ptr<Node> node) {
    int maxPins = std::max(node->InputPins.size(), node->OutputPins.size());
    return ImVec2(nodeWidth, nodeHeaderHeight + (2 * nodePadding) + (maxPins - 1) * pinYSpacing);
}

ImVec2 MaterialEditor::CalculateNodePosition(std::shared_ptr<Node> node) {
    return ImVec2(node->CanvasPosition.x + m_CanvasScreenPos.x + m_CanvasScrolling.x, node->CanvasPosition.y + m_CanvasScreenPos.y + m_CanvasScrolling.y);
}

ImVec2 MaterialEditor::CalculatePinPosition(std::shared_ptr<Node> node, unsigned int pinIndex, bool isInputPin) {
    ImVec2 nodePosition = CalculateNodePosition(node);
    ImVec2 nodeSize = CalculateNodeSize(node);
    ImVec2 pinPosition = 
        (isInputPin) ?
        ImVec2(nodePosition.x, nodePosition.y + nodeHeaderHeight + nodePadding + pinYSpacing * pinIndex) :
        ImVec2(nodePosition.x + nodeSize.x, nodePosition.y + nodeHeaderHeight + nodePadding + pinYSpacing * pinIndex);
    return pinPosition;
}

std::shared_ptr<Node> MaterialEditor::FindNodeById(IcePick::UUID nodeId) {
    for (std::shared_ptr<Node> node : m_EditMaterialNodeGraph) {
        if (node->Id == nodeId)
            return node;
    }

    IP_ASSERT(false, "Invalid Node Id");
    return nullptr;
}

void MaterialEditor::ConnectActivePin(std::shared_ptr<Node> destinationNode, unsigned int destinationPinIndex, bool destinationIsInputPin) {
    m_PinActive = false;
    // trying to connect pins of the same type (input-input, output-output)
    if (destinationIsInputPin == m_IsInputPin)
        return;

    if (destinationNode->Id == m_SourcePinNodeId)
        return;

    if (destinationIsInputPin) {
        std::shared_ptr<Node> sourceNode = FindNodeById(m_SourcePinNodeId);
        if (sourceNode == nullptr) { // how did you achieve this?
            return;
        }

        DisconnectPins(destinationNode, destinationPinIndex, true);
        InputPin& destPin = destinationNode->InputPins[destinationPinIndex];
        destPin.ConnectedNodeId = m_SourcePinNodeId;
        destPin.ConnectedPinIndex = m_SourcePinIndex;

        OutputPin& sourcePin = sourceNode->OutputPins[m_SourcePinIndex];
        sourcePin.ConnectedNodeIds.push_back(destinationNode->Id);
        sourcePin.ConnectedPinIndices.push_back(destinationPinIndex);
    }
    else {
        std::shared_ptr<Node> sourceNode = FindNodeById(m_SourcePinNodeId);
        if (sourceNode == nullptr) { // how did you achieve this?
            return;
        }

        DisconnectPins(sourceNode, m_SourcePinIndex, true);
        InputPin& sourcePin = sourceNode->InputPins[m_SourcePinIndex];
        sourcePin.ConnectedNodeId = destinationNode->Id;
        sourcePin.ConnectedPinIndex = destinationPinIndex;

        OutputPin& destPin = destinationNode->OutputPins[destinationPinIndex];
        destPin.ConnectedNodeIds.push_back(m_SourcePinNodeId);
        destPin.ConnectedPinIndices.push_back(m_SourcePinIndex);
    }
}

void MaterialEditor::DisconnectPins(std::shared_ptr<Node> node, unsigned int pinIndex, bool isInputPin) {
    if (isInputPin) { // disconnect one connection
        InputPin& pin = node->InputPins[pinIndex];
        IcePick::UUID connectedNodeId = pin.ConnectedNodeId;
        unsigned int connectedPinIndex = pin.ConnectedPinIndex;

        if (!NodeExists(connectedNodeId)) {
            // something weird happened
            return;
        }

        std::shared_ptr<Node> connectedNode = FindNodeById(connectedNodeId);
        OutputPin& connectedPin = connectedNode->OutputPins[connectedPinIndex];
        
        pin.DeleteConnection();
        connectedPin.DeleteConnection(node->Id, pinIndex);
    }
    else { // disconnect every connection
        OutputPin& pin = node->OutputPins[pinIndex];
        for (int connectionIndex = 0; connectionIndex < pin.ConnectedNodeIds.size(); connectionIndex++) {
            IcePick::UUID connectedNodeId = pin.ConnectedNodeIds[connectionIndex];
            unsigned int connectedPinIndex = pin.ConnectedPinIndices[connectionIndex];

            if (!NodeExists(connectedNodeId)) {
                // something weird happened
                continue;
            }

            std::shared_ptr<Node> connectedNode = FindNodeById(connectedNodeId);
            InputPin& connectedInputPin = connectedNode->InputPins[connectedPinIndex];
            connectedInputPin.DeleteConnection();
        }
        pin.ConnectedNodeIds.clear();
        pin.ConnectedPinIndices.clear();
    }
}

void MaterialEditor::CompileMaterial() {
    m_NodeIdentifiers.clear();
    for (auto node : m_EditMaterialNodeGraph) {
        node->Unitialise();
    }

    std::stringstream ss;
    CreateShaderFromGraph(ss, m_EditMaterialNodeGraph[0], 0);
    IP_LOG(ss.str());
}

std::string MaterialEditor::CreateShaderFromGraph(std::stringstream& ss, std::shared_ptr<Node> node, unsigned int outputPinIndex) {

    // Get current node input values
    for (InputPin& pin : node->InputPins) {
        if (pin.ConnectedNodeId == IcePick::UUID::Unitialised()) {
            pin.ShaderIdentifier = Pin::GetPinDefault(pin.Type);
            continue;
        }

        if (!NodeExists(pin.ConnectedNodeId)) {
            IP_LOG("Error compiling graph. Node not found.", IP_ERROR_LOG);
            break;
        }

        std::shared_ptr<Node> connectedNode = FindNodeById(pin.ConnectedNodeId);
        pin.ShaderIdentifier = CreateShaderFromGraph(ss, connectedNode, pin.ConnectedPinIndex);
    }

    node->Initialise(ss);
    node->ParseNodeLogic(ss);
    return node->GetPinOutput(outputPinIndex);
}

MaterialEditor::~MaterialEditor() {

}


