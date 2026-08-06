#include "EditorLayer.h"
#include "Event Systems/Input.h"
#include "Utilities/DebugStatistics.h"
#include "LogSystem.h"
#include <imgui-docking/ImGuizmo.h>
#include <IconsFontAwesome7.h>

IcePick::EditorLayer::EditorLayer(EngineAPI engineAPI) :
    m_EngineAPI(engineAPI),
    m_Toolbar(engineAPI),
    m_Viewport(engineAPI),
    m_AssetBrowser(engineAPI),
    m_MaterialEditor(engineAPI),
    m_PropertiesPanel(engineAPI)
{
    auto selectionContextChangeCallback = std::bind(&EditorLayer::OnChangeSelectionContext, this, std::placeholders::_1);
    auto editMaterialCallback = std::bind(&EditorLayer::OnChangeEditMaterial, this, std::placeholders::_1);
    m_Viewport.SetSelectionContextChangeCallback(selectionContextChangeCallback);
    m_ScenePanel.SetSelectionContextChangeCallback(selectionContextChangeCallback);
    m_AssetBrowser.SetEditMaterialCallback(editMaterialCallback);
}

void IcePick::EditorLayer::OnAttach() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking
#ifdef _WIN64
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif
    io.Fonts->AddFontDefault();

    ImGui::StyleColorsDark();
    m_Styles.Init(m_EngineAPI);
    m_AssetBrowser.Init(m_EngineAPI, m_Styles);

    GLFWwindow* window = IcePickRenderer::GetRendererWindow();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void IcePick::EditorLayer::OnChangeSelectionContext(SelectionContext selectionContext) {
    m_Viewport.SetSelectionContext(selectionContext);
    m_ScenePanel.SetSelectionContext(selectionContext);
    m_PropertiesPanel.SetSelectionContext(selectionContext);
}

void IcePick::EditorLayer::OnChangeEditMaterial(std::filesystem::path editMaterialPath) {
    m_MaterialEditor.SetEditMaterial(editMaterialPath);
}

void IcePick::EditorLayer::OnUpdate(DeltaTime dt) {
    m_Viewport.OnUpdate(dt);
    m_DopeSheet.OnUpdate(dt);
    m_ScenePanel.OnUpdate(dt);
    m_MaterialEditor.OnUpdate(dt);
}


void IcePick::EditorLayer::OnEvent(Event& event) {
    m_Viewport.OnViewportEvent(event);
}

void IcePick::EditorLayer::OnPreRender() {
    // Scene space editor rendering
    //glm::vec3 point1 = glm::vec3(0.0f);
    //glm::vec3 point2 = glm::vec3(4.0f);

    //m_EngineAPI.RenderLine(point1, point2, glm::vec4(2.0f));
}

void IcePick::EditorLayer::OnRender(RenderPayload& payload) {
    m_EngineAPI.SetRenderTargetDefault();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    IP_CORE_PROFILE_BEGIN("Editor layer");
    ImGui::PushFont(m_Styles.GetLargeFont());
    ImGuiViewport* mainViewPort = ImGui::GetMainViewport();
    ImGuiID dockspace_id = ImGui::GetID("EditorDockSpace");
    ImGui::DockSpaceOverViewport(dockspace_id, mainViewPort, ImGuiDockNodeFlags_PassthruCentralNode);

    m_Toolbar.Render();
    ImGui::ShowDemoWindow();
    m_LogPanel.RenderLogs();
    m_ScenePanel.ShowSceneHierarchy(m_EngineAPI);
    m_AssetBrowser.Render();
    m_DopeSheet.Render();

    m_PropertiesPanel.SetDropEntity(m_ScenePanel.GetDraggedEntity());
    m_PropertiesPanel.SetDropAssetPath(m_AssetBrowser.GetDragFilePath());
    m_Viewport.SetDropAssetPath(m_AssetBrowser.GetDragFilePath());
    m_MaterialEditor.SetDropAssetPath(m_AssetBrowser.GetDragFilePath());

    m_PropertiesPanel.SelectedProperties(m_Styles);
    m_MaterialEditor.Render();
    m_Viewport.Render(payload.FrameBufferID);
    IP_CORE_PROFILE_POP();
    m_StatisticsPanel.ShowStats();

    ImGui::PopFont();

    IP_CORE_PROFILE_BEGIN("ImGui render");
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    IP_CORE_PROFILE_POP();

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
#ifdef _WIN64
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
#else
        IP_ASSERT(false, "Unsupported platform.");
#endif
        IcePickRenderer::SetCurrentContext(IcePickRenderer::GetRendererWindow());        
    }
}


void IcePick::EditorLayer::OnDetach() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}