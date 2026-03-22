#include "PrefabEditor.h"

PrefabEditor::PrefabEditor(IcePick::EngineAPI engineAPI) :
	m_EngineAPI(engineAPI),
	m_PreviewRenderer(engineAPI)
{
    glm::ivec2 windowSize = IcePickRenderer::GetRendererWindowSize();
    m_PreviewViewportSizeX = windowSize.x;
    m_PreviewViewportSizeY = windowSize.y;
	m_PreviewRenderer.Init(m_PreviewViewportSizeX, m_PreviewViewportSizeY);
}

void PrefabEditor::SetEditPrefab(entt::entity prefabId) {
	m_EditPrefabId = prefabId;
}

void PrefabEditor::SetDropAssetPath(std::filesystem::path filePath) {
    m_DropAssetPath = filePath;
}

void PrefabEditor::Render(const Styles& styles) {
    if (!m_Open) {
        return;
    }
    ImGui::Begin(m_Title, &m_Open, ImGuiWindowFlags_NoCollapse);

    if (ImGui::BeginTable("Prefab", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable)) {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextRow(ImGuiTableRowFlags_None);
        ImGui::TableNextColumn();

        if (ImGui::Button("Prefab button.")) {
            IP_LOG("Pressed button.");
        }


        ImGui::TableNextColumn();

        m_PreviewRenderer.Clear();
        ImVec2 availableSpace = ImGui::GetContentRegionAvail();
        ImGui::Image((void*)m_PreviewRenderer.GetRenderTexture(), availableSpace, ImVec2(0, 1), ImVec2(1, 0));
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsItemHovered()) {
            IcePickRenderer::RequestCursorLock();
            //previewWindowRightClicked = true;
            //lockCursorFirstFrame = true;
        }
        else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
            IcePickRenderer::RequestCursorUnlock();
            //previewWindowRightClicked = false;
        }


        ImGui::EndTable();
    }

	ImGui::End();
}

PrefabEditor::~PrefabEditor() {
    m_PreviewRenderer.Destroy();
}
