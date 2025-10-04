#include "MaterialEditor.h"
#include "imgui-docking/imgui.h"
#include "../LogSystem.h"
#include "../Utilities/Assert.h"

MaterialEditor::MaterialEditor(IcePick::EngineAPI engineAPI) :
	m_EngineAPI(engineAPI),
	m_Renderer(engineAPI)
{
    m_CanvasScrolling = ImVec2(0.0f, 0.0f);
}

void MaterialEditor::SetEditMaterial(IcePick::UUID materialID) {
	m_Open = true;
	m_EditMaterialId = materialID;

    m_EditMaterialNodeGraph.clear();
    m_EditMaterialNodeGraph.push_back(std::make_shared<BSDFNode>());
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
		const int imageSize = 300;
		ImGui::ImageButton("##MaterialButton", (void*)1, ImVec2(imageSize, imageSize), ImVec2(0, 1), ImVec2(1, 0));

		ImGui::TableNextColumn();
		DrawCanvas();
		ImGui::EndTable();
	}
	ImGui::End();
}

void MaterialEditor::DrawCanvas() {
    //static ImVec2 scrolling(0.0f, 0.0f);

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
            m_EditMaterialNodeGraph.push_back(std::make_shared<TextureNode>(IcePick::UUID::Unitialised()));
        }
        if (ImGui::MenuItem("Vector3 Node", NULL, false)) { 
            
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

    float nodeWidth = 300.0f;
    float nodePadding = 50.0f;
    float nodeHeaderHeight = 30.0f;
    float nodeCornerRounding = 9.0f;
    float pinYSpacing = 20.0f;
    ImU32 nodeBgColour = IM_COL32(10, 10, 10, 255);
    ImU32 nodeHeaderColour = IM_COL32(90, 60, 150, 255);

    for (int i = 0; i < m_EditMaterialNodeGraph.size(); i++) {
        ImGui::PushID(i);
        std::shared_ptr<Node> node = m_EditMaterialNodeGraph[i];

        ImVec2 nodePosition = ImVec2(node->CanvasPosition.x + m_CanvasScreenPos.x + m_CanvasScrolling.x, node->CanvasPosition.y + m_CanvasScreenPos.y + m_CanvasScrolling.y);
        int maxPins = std::max(node->InputPins.size(), node->OutputPins.size());
        ImVec2 nodeSize = ImVec2(nodeWidth, (2 * nodePadding) + (maxPins - 1) * pinYSpacing);

        draw_list->AddRectFilled(nodePosition, ImVec2(nodePosition.x + nodeSize.x, nodePosition.y + nodeSize.y), nodeBgColour, nodeCornerRounding, ImDrawFlags_RoundCornersAll);
        draw_list->AddRectFilled(nodePosition, ImVec2(nodePosition.x + nodeSize.x, nodePosition.y + nodeHeaderHeight), nodeHeaderColour, nodeCornerRounding, ImDrawFlags_RoundCornersTop);

        ImGui::SetCursorScreenPos(nodePosition);
        ImGui::InvisibleButton("nodeHeader", ImVec2(nodeSize.x, nodeHeaderHeight), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
        const bool nodeHeaderHovered = ImGui::IsItemHovered();
        const bool nodeHeaderHeld = ImGui::IsItemActive();

        if (nodeHeaderHeld) {
            IP_LOG("Header held");
        }

        if (nodeHeaderHeld && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            node->CanvasPosition.x += io.MouseDelta.x;
            node->CanvasPosition.y += io.MouseDelta.y;
        }
        ImGui::PopID();
    }
}

void MaterialEditor::DrawNodeConnections() {
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddCircle(ImVec2(1000.0f, 400.0f), 20.0f, 0xFFFFFFFF, 20, 3);
    draw_list->AddBezierCubic(ImVec2(1000.0f, 400.0f), ImVec2(1030.0f, 400.0f), ImVec2(1060.0f, 460.0f), ImVec2(1090.0f, 460.0f), 0xFFFFFFFF, 3, 20);
}

bool MaterialEditor::NodeExists(IcePick::UUID nodeId) {
    for (const std::shared_ptr<Node> node : m_EditMaterialNodeGraph) {
        if (node->Id == nodeId)
            return true;
    }
    return false;
}

std::shared_ptr<Node> MaterialEditor::FindNodeById(IcePick::UUID nodeId) {
    for (std::shared_ptr<Node> node : m_EditMaterialNodeGraph) {
        if (node->Id == nodeId)
            return node;
    }

    IP_ASSERT(false, "Invalid Node Id");
    return nullptr;
}

MaterialEditor::~MaterialEditor() {

}


