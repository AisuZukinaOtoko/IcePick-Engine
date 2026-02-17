#include "../src/IcePickCoreMinimal.h"
#include "EditorLayer.h"
#include "../src/Event Systems/Input.h"
#include "../src/Utilities/DebugStatistics.h"
#include "imgui-docking/ImGuizmo.h"
#include "IconsFontAwesome4.h"

IcePick::EditorLayer::EditorLayer(EngineAPI engineAPI) :
    m_EngineAPI(engineAPI),
    m_Toolbar(engineAPI),
    m_Viewport(engineAPI),
    m_AssetBrowser(engineAPI),
    m_MaterialEditor(engineAPI),
    m_PropertiesPanel(engineAPI)
{
    auto entityChangeCallback = std::bind(&EditorLayer::OnChangeSelectedEntity, this, std::placeholders::_1);
    auto editMaterialCallback = std::bind(&EditorLayer::OnChangeEditMaterial, this, std::placeholders::_1);
    m_Viewport.SetSelectedEntityChangeCallback(entityChangeCallback);
    m_ScenePanel.SetSelectedEntityChangeCallback(entityChangeCallback);
    m_AssetBrowser.SetEditMaterialCallback(editMaterialCallback);
}

void IcePick::EditorLayer::OnAttach() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.Fonts->AddFontDefault();

    ImFontConfig config;
    config.MergeMode = true;
    config.PixelSnapH = true;

    ImGui::StyleColorsDark();
    m_Styles.Init(m_EngineAPI);
    m_AssetBrowser.Init(m_EngineAPI, m_Styles);
    m_EngineAPI.SetEngineRuntimeState(IcePick::RuntimeState::STOPPED);

    std::filesystem::path fontPath = "res/Fonts/freesans-font/FreeSans-LrmZ.ttf";
    std::filesystem::path iconFontPath = "res/Fonts/fontawesome-free-7.2.0-desktop/otfs/Font Awesome 7 Free-Solid-900.otf";
    m_EditorFont = io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 18.0f);

    static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

    io.Fonts->AddFontFromFileTTF(
        iconFontPath.string().c_str(), 18.0f,
        &config, icon_ranges
    );

    io.Fonts->Build();
    if (!m_EditorFont)
        IP_LOG("Failed to load editor font: " + fontPath.string(), IP_WARN_LOG);

    GLFWwindow* window = IcePickRenderer::GetRendererWindow();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void IcePick::EditorLayer::OnChangeSelectedEntity(entt::entity selectedEntity) {
    m_Viewport.SetSelectedEntity(selectedEntity);
    m_ScenePanel.SetSelectedEntity(selectedEntity);
    m_PropertiesPanel.SetSelectedEntity(selectedEntity);
}

void IcePick::EditorLayer::OnChangeEditMaterial(std::filesystem::path editMaterialPath) {
    m_MaterialEditor.SetEditMaterial(editMaterialPath);
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

    IP_CORE_PROFILE_BEGIN("Editor layer");
    ImGui::PushFont(m_EditorFont);
    ImGuiViewport* mainViewPort = ImGui::GetMainViewport();
    ImGuiID dockspace_id = ImGui::GetID("EditorDockSpace");
    ImGui::DockSpaceOverViewport(dockspace_id, mainViewPort, ImGuiDockNodeFlags_PassthruCentralNode);

    m_Toolbar.Render();
    //ImGui::ShowDemoWindow();
    m_LogPanel.RenderLogs();
    m_ScenePanel.ShowSceneHierarchy();
    m_AssetBrowser.Render();

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
}

