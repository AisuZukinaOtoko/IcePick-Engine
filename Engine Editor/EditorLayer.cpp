#include "../src/IcePickCoreMinimal.h"
#include "EditorLayer.h"
#include "../src/Event Systems/Input.h"
#include "imgui-docking/ImGuizmo.h"

IcePick::EditorLayer::EditorLayer(EngineAPI engineAPI) :
    m_EngineAPI(engineAPI),
    m_Toolbar(engineAPI),
    m_Viewport(engineAPI),
    m_AssetBrowser(engineAPI),
    m_MaterialEditor(engineAPI)
{
    m_Styles.Init(engineAPI);
    m_AssetBrowser.Init(m_EngineAPI, m_Styles);

    auto entityChangeCallback = std::bind(&EditorLayer::OnChangeSelectedEntity, this, std::placeholders::_1);
    auto editMaterialCallback = std::bind(&EditorLayer::OnChangeEditMaterial, this, std::placeholders::_1);
    m_Viewport.SetSelectedEntityChangeCallback(entityChangeCallback);
    m_ScenePanel.SetSelectedEntityChangeCallback(entityChangeCallback);
    m_PropertiesPanel.SetEditMaterialCallback(editMaterialCallback);
    m_AssetBrowser.SetEditMaterialCallback(editMaterialCallback);
}

void IcePick::EditorLayer::OnAttach() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.Fonts->AddFontDefault();

    std::filesystem::path fontPath = "res/Fonts/freesans-font/FreeSans-LrmZ.ttf";
    m_EditorFont = io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 18.0f);
    io.Fonts->Build();
    if (!m_EditorFont)
        IP_LOG("Failed to load editor font: " + fontPath.string(), IP_WARN_LOG);

    ImGui::StyleColorsDark();
    GLFWwindow* window = IcePickRenderer::GetRendererWindow();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void IcePick::EditorLayer::OnChangeSelectedEntity(entt::entity selectedEntity) {
    m_Viewport.SetSelectedEntity(selectedEntity);
    m_ScenePanel.SetSelectedEntity(selectedEntity);
    m_PropertiesPanel.SetSelectedEntity(selectedEntity);
}

void IcePick::EditorLayer::OnChangeEditMaterial(UUID editMaterialID) {
    m_MaterialEditor.SetEditMaterial(editMaterialID);
}

void IcePick::EditorLayer::OnUpdate(DeltaTime dt) {
    m_Viewport.OnUpdate(dt);
    m_ScenePanel.OnUpdate(dt);
    m_MaterialEditor.OnUpdate(dt);
}

void IcePick::EditorLayer::OnDetach() {

}

void IcePick::EditorLayer::OnEvent(Event& event) {
    m_Viewport.OnViewportEvent(event);
}

void IcePick::EditorLayer::OnRender(RenderPayload& payload) {
    m_EngineAPI.SetRenderTargetDefault();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    ImGui::PushFont(m_EditorFont);
    ImGuiViewport* mainViewPort = ImGui::GetMainViewport();
    ImGuiID dockspace_id = ImGui::GetID("EditorDockSpace");
    ImGui::DockSpaceOverViewport(dockspace_id, mainViewPort, ImGuiDockNodeFlags_PassthruCentralNode);

    m_Toolbar.Render();
    ImGui::ShowDemoWindow();
    m_LogPanel.RenderLogs();
    m_ScenePanel.ShowSceneHierarchy();
    m_AssetBrowser.Render();

    m_PropertiesPanel.SetDropAssetPath(m_AssetBrowser.GetDragFilePath());
    m_Viewport.SetDropAssetPath(m_AssetBrowser.GetDragFilePath());
    m_MaterialEditor.SetDropAssetPath(m_AssetBrowser.GetDragFilePath());

    m_PropertiesPanel.SelectedProperties(m_Styles);
    m_MaterialEditor.Render();
    m_Viewport.Render(payload.FrameBufferID);
    m_StatisticsPanel.ShowStats();

    ImGui::PopFont();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

