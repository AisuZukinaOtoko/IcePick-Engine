#include "MaterialEditor.h"
#include "Nodes/Nodes.h"
#include "imgui-docking/imgui.h"
#include "../LogSystem.h"
#include "../Utilities/Assert.h"
#include "../Utils/Serialize.h"

static bool RenameNodeTextNeedsFocus = false;
static glm::vec4 tempColour = glm::vec4(0.0f);

MaterialEditor::MaterialEditor(IcePick::EngineAPI engineAPI) :
	m_EngineAPI(engineAPI),
	m_Renderer(engineAPI)
{
    Pin::InitPins();
    m_CanvasScrolling = ImVec2(0.0f, 0.0f);
    m_Renderer.editorCamera.aspectRatio = 1.0f;
    m_Renderer.Init(previewImageSize, previewImageSize);
    previewMesh = m_EngineAPI.LoadMesh("res/Assets/sphere.glb");

    m_EditMaterialNodeGraph.push_back(std::make_shared<BSDFNode>());

    m_MaterialEditorMaterialBaseId = m_EngineAPI.RegisterMaterialBase(m_MaterialEditorMaterialBase);
    m_MaterialEditorMaterialInstance.MaterialBaseId = m_MaterialEditorMaterialBaseId;
    m_MaterialEditorMaterialInstanceId = m_EngineAPI.RegisterMaterialInstance(m_MaterialEditorMaterialInstance);

    m_MaterialEditorShaderSourceTemplate.VertexShaderSource = m_EngineAPI.LoadShaderSourceFile("res/shaders/pbr.vert.shader");
    m_MaterialEditorShaderSourceTemplate.FragmentShaderSource = m_EngineAPI.LoadShaderSourceFile("res/shaders/gpass.frag.shader");

    IcePick::ShaderSource newShaderSource = GetShaderSourceFromGraph();
    m_MaterialEditorShaderId = m_EngineAPI.CreateShaderFromSource(newShaderSource);
    m_MaterialEditorMaterialBase.ShaderId = m_MaterialEditorShaderId;
}

void MaterialEditor::SetDropAssetPath(std::filesystem::path filePath) {
    m_DropAssetPath = filePath;
}

void MaterialEditor::SetEditMaterial(std::filesystem::path materialBasePath) {
	m_Open = true;

    m_EditMaterialBasePath = materialBasePath;
    m_EditMaterialBaseId = m_EngineAPI.LoadMaterialBaseFromAsset(m_EditMaterialBasePath);
    const IcePick::MaterialBase& editMaterialBase = m_EngineAPI.GetMaterialBase(m_EditMaterialBaseId);
    m_EditMaterialShaderId = editMaterialBase.ShaderId;

    m_EditMaterialNodeGraph.clear();
    m_EditMaterialNodeGraph = LoadMaterialBaseEditorData(materialBasePath, nullptr);

    // material graph should never have 0 nodes
    if (!m_EditMaterialNodeGraph.size()) {
        m_EditMaterialNodeGraph.push_back(std::make_shared<BSDFNode>());
    }

    CompileMaterial();
    m_MaterialEditorMaterialBase.ClearShaderInputs();
    m_MaterialEditorMaterialBase.ClearMaterialBaseData();
    m_MaterialEditorMaterialInstance.ClearMaterialInstanceData();
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
		ImGui::Image((void*)m_Renderer.GetRenderTexture(), ImVec2(previewImageSize, previewImageSize), ImVec2(0, 1), ImVec2(1, 0));
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsItemHovered()) {
            IcePickRenderer::RequestCursorLock();
            previewWindowRightClicked = true;
            lockCursorFirstFrame = true;
        }
        else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            IcePickRenderer::RequestCursorUnlock();
            previewWindowRightClicked = false;
        }

        ImGui::Checkbox("Auto compile graph", &m_AutoCompileGraph);
        ImGui::SameLine();
        if (ImGui::Button("Compile graph")) {
            CompileMaterial();
        }

        if (ImGui::Button("Save Material")) {
            IP_LOG("Saved material base.");
            IcePick::ShaderSource newShaderSource = GetShaderSourceFromGraph();
            IcePick::MaterialBase saveMaterialBase = m_MaterialEditorMaterialBase;
            saveMaterialBase.Id = m_EditMaterialBaseId;
            saveMaterialBase.ShaderId = m_EditMaterialShaderId;

            m_EngineAPI.UpdateShaderWithSource(m_EditMaterialShaderId, newShaderSource);
            m_EngineAPI.UpdateMaterialBase(m_EditMaterialBaseId, saveMaterialBase);
            SerializeMaterialBase(m_EditMaterialBasePath, saveMaterialBase, m_EditMaterialNodeGraph, newShaderSource);
        }

        ImGui::Separator();
        ShowEditMaterialBaseParameters();

        ImGui::ColorEdit3("##Colour", &tempColour.r, ImGuiColorEditFlags_PickerHueWheel);

		ImGui::TableNextColumn();
		DrawCanvas();
		ImGui::EndTable();
	}
	ImGui::End();
}

void MaterialEditor::PreviewMaterial() {
    bool nodeChangesMade = false;
    for (auto& node : m_EditMaterialNodeGraph) {
        if (node->MaterialBaseStateChanged || node->MaterialInstanceStateChanged) {
            nodeChangesMade = true;
            node->MaterialBaseStateChanged = false;
            node->MaterialInstanceStateChanged = false;
        }
    }

    if (m_AutoCompileGraph && (m_GraphStateChanged || nodeChangesMade)) {
        CompileMaterial();
        m_GraphStateChanged = false;
    }

    m_Renderer.Clear();

    previewMesh.MaterialSlots[0] = m_MaterialEditorMaterialInstanceId;
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

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
    if (ImGui::BeginPopup("context")) {
        ShowAddNodeOptions(mouse_pos_in_canvas);
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();

    // Draw grid + all lines in the canvas
    draw_list->PushClipRect(canvas_p0, canvas_p1, true);
    const float GRID_STEP = 64.0f;
    for (float x = fmodf(m_CanvasScrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
        draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
    for (float y = fmodf(m_CanvasScrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
        draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));

    DrawNodeConnections();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
    DrawNodes();
    ImGui::PopStyleVar();
    

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

    for (int i = 0; i < m_EditMaterialNodeGraph.size(); i++) {
        ImGui::PushID(i);
        std::shared_ptr<Node> node = m_EditMaterialNodeGraph[i];

        ImVec2 nodePosition = CalculateNodePosition(node);
        ImVec2 nodeSize = CalculateNodeSize(node);

        draw_list->AddRectFilled(nodePosition, ImVec2(nodePosition.x + nodeSize.x, nodePosition.y + nodeSize.y), m_RenderInfo.NodeBgColour, m_RenderInfo.NodeCornerRounding, ImDrawFlags_RoundCornersAll);
        draw_list->AddRectFilled(nodePosition, ImVec2(nodePosition.x + nodeSize.x, nodePosition.y + m_RenderInfo.NodeHeaderHeight), node->NodeHeaderColour, m_RenderInfo.NodeCornerRounding, ImDrawFlags_RoundCornersTop);

        ImVec2 nodeTitleTextSize = ImGui::CalcTextSize(node->NodeName.c_str());
        ImVec2 nodeTitleTextPosition = ImVec2(nodePosition.x + m_RenderInfo.LabelPadding, nodePosition.y + ((m_RenderInfo.NodeHeaderHeight - nodeTitleTextSize.y) / 2.0f));
        if (node->Id != m_RenameNodeId)
            draw_list->AddText(NULL, 20.0f, nodeTitleTextPosition, IM_COL32(255, 255, 255, 255), node->NodeName.c_str());

        ImGui::SetCursorScreenPos(nodePosition);
        ImGui::SetNextItemAllowOverlap();
        ImGui::InvisibleButton("nodeHeader", ImVec2(nodeSize.x, m_RenderInfo.NodeHeaderHeight), ImGuiButtonFlags_MouseButtonLeft);
        const bool nodeHeaderHovered = ImGui::IsItemHovered();
        const bool nodeHeaderHeld = ImGui::IsItemActive();
        const bool nodeHeaderRightClicked = nodeHeaderHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right);

        char inputTextBuffer[30] = {};
        if (node->Id == m_RenameNodeId) {
            ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(25, 25, 25, 120));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);

            ImGui::SetCursorScreenPos(ImVec2(nodeTitleTextPosition.x, nodeTitleTextPosition.y - (m_RenderInfo.LabelPadding / 2.0f)));
            ImGui::SetNextItemWidth(node->NodeWidth - (m_RenderInfo.LabelPadding * 2));
            if (RenameNodeTextNeedsFocus) {
                ImGui::SetKeyboardFocusHere();
                RenameNodeTextNeedsFocus = false;
            }
            ImGui::InputText("##RenameNodeTextInput", inputTextBuffer, sizeof(inputTextBuffer));
            if (ImGui::IsItemDeactivated()) {
                node->NodeName = inputTextBuffer;
                m_RenameNodeId = IcePick::UUID::Unitialised();
                m_GraphStateChanged = true; //to update the labels on the edit widgets
            }

            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }

        if (nodeHeaderRightClicked) {
            ImGui::OpenPopup("NODE_OPTIONS");
        }

        if (ImGui::BeginPopup("NODE_OPTIONS")) {            
            ShowNodeEditOptions(node);
            ImGui::EndPopup();
        }
        

        for (int j = 0; j < node->InputPins.size(); j++) {
            ImGui::PushID(j);
            ImVec2 pinPosition = CalculatePinPosition(node, j, true);

            std::string& label = node->InputPins[j].Label;
            ImVec2 labelSize = ImGui::CalcTextSize(label.c_str());
            ImVec2 labelPosition = ImVec2(pinPosition.x + m_RenderInfo.PinRadius + m_RenderInfo.LabelPadding, pinPosition.y - (labelSize.y / 2.0f));
            draw_list->AddText(labelPosition, m_RenderInfo.LabelColour, label.c_str());

            bool mouseInRectX = (mousePos.x > pinPosition.x - m_RenderInfo.PinRadius) && (mousePos.x < labelPosition.x + labelSize.x);
            bool mouseInRectY = (mousePos.y > labelPosition.y) && (mousePos.y < labelPosition.y + labelSize.y);
            bool currentPinHovered = mouseInRectX && mouseInRectY;

            ImU32 colour = (currentPinHovered) ? m_RenderInfo.NodePinColourHovered : m_RenderInfo.NodePinColour;
            draw_list->AddCircleFilled(pinPosition, m_RenderInfo.PinRadius, colour, m_RenderInfo.PinSegments);
            ImVec2 cursorPos = ImVec2(pinPosition.x - m_RenderInfo.PinRadius, pinPosition.y - m_RenderInfo.PinRadius);
            ImGui::SetCursorScreenPos(cursorPos);
            ImGui::InvisibleButton("inputNodePin", ImVec2(labelPosition.x + labelSize.x - cursorPos.x, labelSize.y), ImGuiButtonFlags_MouseButtonLeft);
            bool currentPinActive = ImGui::IsItemActive();


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

            std::string& label = node->OutputPins[j].Label;
            ImVec2 labelSize = ImGui::CalcTextSize(label.c_str());
            ImVec2 labelPosition = ImVec2(pinPosition.x - m_RenderInfo.PinRadius - m_RenderInfo.LabelPadding - labelSize.x, pinPosition.y - (labelSize.y / 2.0f));
            draw_list->AddText(labelPosition, IM_COL32(255, 255, 255, 255), label.c_str());

            bool mouseInRectX = (mousePos.x < (pinPosition.x + m_RenderInfo.PinRadius)) && (mousePos.x > labelPosition.x);
            bool mouseInRectY = (mousePos.y > labelPosition.y) && (mousePos.y < labelPosition.y + labelSize.y);
            bool currentPinHovered = mouseInRectX && mouseInRectY;

            ImU32 colour = (currentPinHovered) ? m_RenderInfo.NodePinColourHovered : m_RenderInfo.NodePinColour;
            draw_list->AddCircleFilled(pinPosition, m_RenderInfo.PinRadius, colour, m_RenderInfo.PinSegments);
            ImGui::SetCursorScreenPos(labelPosition);
            ImGui::InvisibleButton("outputNodePin", ImVec2(pinPosition.x + m_RenderInfo.PinRadius - labelPosition.x, labelSize.y), ImGuiButtonFlags_MouseButtonLeft);
            bool currentPinActive = ImGui::IsItemActive();


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

        node->CustomRendering(m_EngineAPI, m_DropAssetPath, m_RenderInfo, m_CanvasScreenPos, m_CanvasScrolling);

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
    float innerPointXOffset = std::abs(lineEnd.x - lineStart.x) / 3.0f + 10.0f;
    float innerPointYOffset = (lineEnd.y - lineStart.y) / 8.0f;
    
    ImVec2 lineP2 = ImVec2(lineStart.x + innerPointXOffset * sign, lineStart.y + innerPointYOffset);
    ImVec2 lineP3 = ImVec2(lineEnd.x - innerPointXOffset * sign, lineEnd.y - innerPointYOffset);

    draw_list->AddBezierCubic(lineStart, lineP2, lineP3, lineEnd, m_RenderInfo.NodePinColour, m_RenderInfo.LineThickness, m_RenderInfo.LineSegments);
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

void MaterialEditor::ShowEditMaterialBaseParameters() {
    ImGui::BeginChild("EditMaterialBaseParameters");
    if (ImGui::BeginTable("Texture parameters", 1)) {
        ImGui::TableNextRow(ImGuiTableRowFlags_None);
        ImGui::TableSetColumnIndex(0);

        //for (int i = 0; i < m_MaterialEditorMaterialBase.MaterialTextures.size(); i++) {
        //    ImGui::PushID(i);
        //    auto& baseTextureParam = m_MaterialEditorMaterialBase.MaterialTextures[i];
        //    IcePick::UUID baseDataId = baseTextureParam.Id;
        //    IcePick::UUID parameterTextureId = m_MaterialEditorMaterialInstance.GetMaterialInstanceTextureId(baseDataId);
        //    unsigned int parameterTextureRenderId = m_EngineAPI.GetTextureRenderId(parameterTextureId);

        //    if (ImGui::BeginTable("Material Texture", 2)) {
        //        ImGui::TableNextRow(ImGuiTableRowFlags_None);
        //        ImGui::TableNextColumn();
        //        const int imageSize = 45;
        //        ImGui::Image((void*)parameterTextureRenderId, ImVec2(imageSize, imageSize), ImVec2(0, 1), ImVec2(1, 0));

        //        ImGui::TableNextColumn();

        //        ImGui::Text(baseTextureParam.DisplayName.c_str());
        //        if (ImGui::Button("Clear")) {
        //            m_MaterialEditorMaterialInstance.SetMaterialInstanceTextureId(baseDataId, IcePick::UUID::Unitialised());
        //            m_EngineAPI.UpdateMaterialInstance(m_MaterialEditorMaterialInstanceId, m_MaterialEditorMaterialInstance); // Calling update will invalidate the cache. This is desired.
        //        }
        //        ImGui::EndTable();
        //    }

        //    if (ImGui::BeginDragDropTarget()) {
        //        if (ImGui::AcceptDragDropPayload("TEXTURE_ASSET")) {
        //            IcePick::UUID droppedTextureId = m_EngineAPI.LoadTextureFromAsset(m_DropAssetPath);
        //            m_MaterialEditorMaterialInstance.SetMaterialInstanceTextureId(baseDataId, droppedTextureId);
        //            m_EngineAPI.UpdateMaterialInstance(m_MaterialEditorMaterialInstanceId, m_MaterialEditorMaterialInstance); // Calling update will invalidate the cache. This is desired.
        //        }
        //        ImGui::EndDragDropTarget();
        //    }
        //    ImGui::PopID();
        //}

        for (int i = 0; i < m_MaterialEditorMaterialInstance.InstanceFloatData.size(); i++) {
            ImGui::PushID(i);
            auto& instanceFloatData = m_MaterialEditorMaterialInstance.InstanceFloatData[i];

            if (ImGui::BeginTable("Instance Float Parameter", 2)) {
                ImGui::TableNextRow(ImGuiTableRowFlags_None);
                ImGui::TableNextColumn();

                std::string parameterName = "Float parameter.";
                for (const auto& baseFloatData : m_MaterialEditorMaterialBase.MaterialFloatParameters) {
                    if (baseFloatData.Id == instanceFloatData.MaterialBaseDataId) {
                        parameterName = baseFloatData.DisplayName;
                    }
                }
                ImGui::Text(parameterName.c_str());
                ImGui::TableNextColumn();

                ImGui::SetNextItemWidth(-FLT_MIN); // Use all available horizontal space
                if (ImGui::DragFloat("##Material Instance Float", &instanceFloatData.Data, 0.005)) {
                    m_EngineAPI.UpdateMaterialInstance(m_MaterialEditorMaterialInstanceId, m_MaterialEditorMaterialInstance); // update material editor instance for updated previews
                }
                ImGui::EndTable();
            }
            ImGui::PopID();
        }

        ImGui::EndTable();
    }
    ImGui::EndChild();
}

void MaterialEditor::ShowAddNodeOptions(ImVec2 mousePosInCanvas) {

    if (ImGui::BeginMenu("Render Nodes")) {
        if (ImGui::MenuItem("Texture Node", NULL, false)) {
            std::shared_ptr<TextureNode> newNode = std::make_shared<TextureNode>(IcePick::UUID::Unitialised());
            newNode->CanvasPosition = mousePosInCanvas;
            m_EditMaterialNodeGraph.push_back(newNode);
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Math Nodes")) {
        if (ImGui::MenuItem("Float Node", NULL, false)) {
            std::shared_ptr<FloatNode> newNode = std::make_shared<FloatNode>();
            newNode->CanvasPosition = mousePosInCanvas;
            m_EditMaterialNodeGraph.push_back(newNode);
        }

        if (ImGui::MenuItem("Vector3 Node", NULL, false)) {
            std::shared_ptr<Vector3Node> newNode = std::make_shared<Vector3Node>();
            newNode->CanvasPosition = mousePosInCanvas;
            m_EditMaterialNodeGraph.push_back(newNode);
        }

        if (ImGui::MenuItem("Vector4 Node", NULL, false)) {
            std::shared_ptr<Vector4Node> newNode = std::make_shared<Vector4Node>();
            newNode->CanvasPosition = mousePosInCanvas;
            m_EditMaterialNodeGraph.push_back(newNode);
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Input Nodes")) {
        if (ImGui::MenuItem("UV Node", NULL, false)) {
            std::shared_ptr<UVNode> newNode = std::make_shared<UVNode>();
            newNode->CanvasPosition = mousePosInCanvas;
            m_EditMaterialNodeGraph.push_back(newNode);
        }

        ImGui::EndMenu();
    }
}

void MaterialEditor::ShowNodeEditOptions(std::shared_ptr<Node> node) {
    std::string nodeParameterToggleText = (node->nodeIsParameter) ? "Demote from parameter" : "Convert to parameter";
    if (ImGui::MenuItem(nodeParameterToggleText.c_str(), NULL, false, node->nodeCanBeParamterized)) {
        node->nodeIsParameter = !node->nodeIsParameter;
        m_GraphStateChanged = true;
    }

    if (ImGui::MenuItem("Rename node", NULL, false)) {
        m_RenameNodeId = node->Id;
        RenameNodeTextNeedsFocus = true;
    }

    if (ImGui::MenuItem("Delete node", NULL, false)) {
        m_GraphStateChanged = true;
        DeleteNode(node->Id);
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
    return ImVec2(node->NodeWidth, m_RenderInfo.NodeHeaderHeight + (2 * m_RenderInfo.NodePadding) + (maxPins - 1) * m_RenderInfo.PinYSpacing);
}

ImVec2 MaterialEditor::CalculateNodePosition(std::shared_ptr<Node> node) {
    return ImVec2(node->CanvasPosition.x + m_CanvasScreenPos.x + m_CanvasScrolling.x, node->CanvasPosition.y + m_CanvasScreenPos.y + m_CanvasScrolling.y);
}

ImVec2 MaterialEditor::CalculatePinPosition(std::shared_ptr<Node> node, unsigned int pinIndex, bool isInputPin) {
    ImVec2 nodePosition = CalculateNodePosition(node);
    ImVec2 nodeSize = CalculateNodeSize(node);
    ImVec2 pinPosition =
        (isInputPin) ?
        ImVec2(nodePosition.x, nodePosition.y + m_RenderInfo.NodeHeaderHeight + m_RenderInfo.NodePadding + m_RenderInfo.PinYSpacing * pinIndex) :
        ImVec2(nodePosition.x + nodeSize.x, nodePosition.y + m_RenderInfo.NodeHeaderHeight + m_RenderInfo.NodePadding + m_RenderInfo.PinYSpacing * pinIndex);
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
    m_GraphStateChanged = true;

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

void MaterialEditor::DeleteNode(IcePick::UUID nodeId) {
    for (unsigned int i = 1; i < m_EditMaterialNodeGraph.size(); i++) { // Start at index 1 since node at index 0 should never be deleted
        std::shared_ptr<Node>& node = m_EditMaterialNodeGraph[i];
        if (node->Id == nodeId) {
            for (unsigned int pinIndex = 0; pinIndex < node->InputPins.size(); pinIndex++) {
                DisconnectPins(node, pinIndex, true);
            }
            for (unsigned int pinIndex = 0; pinIndex < node->OutputPins.size(); pinIndex++) {
                DisconnectPins(node, pinIndex, false);
            }

            std::shared_ptr<Node> lastNode = m_EditMaterialNodeGraph.back();
            node = lastNode;
            m_EditMaterialNodeGraph.pop_back();
            return;
        }
    }
}

void MaterialEditor::CompileMaterial() {
    IcePick::ShaderSource newShaderSource = GetShaderSourceFromGraph();

    m_EngineAPI.UpdateShaderWithSource(m_MaterialEditorShaderId, newShaderSource);
    m_EngineAPI.UpdateMaterialBase(m_MaterialEditorMaterialBaseId, m_MaterialEditorMaterialBase);
    m_EngineAPI.UpdateMaterialInstance(m_MaterialEditorMaterialInstanceId, m_MaterialEditorMaterialInstance);
}

std::string MaterialEditor::CreateShaderFromGraph(std::stringstream& ss, std::shared_ptr<Node> node, unsigned int outputPinIndex, int recursiveDepth) {
    if (recursiveDepth > 100) {
        IP_LOG("Error compiling graph. Max node recursion exceeded.", IP_ERROR_LOG);
        return "";
    }

    // Get current node input values
    for (InputPin& pin : node->InputPins) {
        if (pin.ConnectedNodeId == IcePick::UUID::Unitialised()) {
            pin.ShaderIdentifier = Pin::GetPinDefault(pin.Type);
            continue;
        }

        if (!NodeExists(pin.ConnectedNodeId)) {
            IP_LOG("Error compiling graph. Node not found.", IP_ERROR_LOG);
            return "";
        }

        std::shared_ptr<Node> connectedNode = FindNodeById(pin.ConnectedNodeId);
        pin.ShaderIdentifier = CreateShaderFromGraph(ss, connectedNode, pin.ConnectedPinIndex, recursiveDepth + 1);
    }

    node->Initialise(ss, m_MaterialEditorMaterialBase, m_MaterialEditorMaterialInstance);
    node->ParseNodeLogic(ss);
    return node->GetPinOutput(outputPinIndex);
}

IcePick::ShaderSource MaterialEditor::GetShaderSourceFromGraph() {
    for (auto& node : m_EditMaterialNodeGraph) {
        node->Unitialise();
    }

    m_MaterialEditorMaterialBase.ClearMaterialBaseData();
    m_MaterialEditorMaterialInstance.ClearMaterialInstanceData();

    std::stringstream uniformSS;
    std::stringstream shaderSS;

    CreateShaderFromGraph(shaderSS, m_EditMaterialNodeGraph[0], 0, 0);
    for (const auto& materialTexture : m_MaterialEditorMaterialBase.MaterialTextures) {
        uniformSS << "uniform sampler2D " << materialTexture.SamplerIdentifier << ";\n";
    }

    for (const auto& materialFloatParameter : m_MaterialEditorMaterialBase.MaterialFloatParameters) {
        uniformSS << "uniform float " << materialFloatParameter.ShaderIdentifier << ";\n";
    }

    std::string fragShader = m_MaterialEditorShaderSourceTemplate.FragmentShaderSource;

    std::string replaceTarget = "#uniforms";
    size_t pos = fragShader.find(replaceTarget);
    if (pos != std::string::npos) {
        fragShader.replace(pos, replaceTarget.length(), uniformSS.str());
    }

    replaceTarget = "#shader";
    pos = fragShader.find(replaceTarget);
    if (pos != std::string::npos) {
        fragShader.replace(pos, replaceTarget.length(), shaderSS.str());
    }

    IcePick::ShaderSource newShaderSource;
    newShaderSource.VertexShaderSource = m_MaterialEditorShaderSourceTemplate.VertexShaderSource;
    newShaderSource.FragmentShaderSource = fragShader;

    return newShaderSource;
}

MaterialEditor::~MaterialEditor() {

}


