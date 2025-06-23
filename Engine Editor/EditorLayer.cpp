#include "../src/IcePickCoreMinimal.h"
#include "EditorLayer.h"
#include "../src/Event Systems/Input.h"

IcePick::EditorLayer::EditorLayer(EngineAPI engineAPI) 
    : m_EngineAPI(engineAPI) {
    m_Styles.Init(engineAPI);
    m_AssetBrowser.Init(m_EngineAPI, m_Styles);
}

void IcePick::EditorLayer::OnAttach() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark();
    GLFWwindow* window = IcePickRenderer::GetRendererWindow();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
	IP_LOG("Hello from the IcePick Editor.");
}

void IcePick::EditorLayer::OnUpdate(DeltaTime dt) {
    m_Viewport.OnUpdate(dt);

    if (m_ScenePanel.EntitySelected()) {
        m_PropertiesPanel.SetEntitySelect(m_ScenePanel.GetSelectedEntity());
    }
}

void IcePick::EditorLayer::OnDetach() {

}

void IcePick::EditorLayer::OnEvent(Event& event) {
    m_Viewport.OnViewportEvent(event);
}

void IcePick::EditorLayer::OnRender(RenderPayload& payload) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind default render target
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGuiViewport* mainViewPort = ImGui::GetMainViewport();
    ImGuiID dockspace_id = ImGui::GetID("EditorDockSpace");
    ImGui::DockSpaceOverViewport(dockspace_id, mainViewPort, ImGuiDockNodeFlags_PassthruCentralNode);

    m_Toolbar.Render();
    ImGui::ShowDemoWindow();
    m_LogPanel.RenderLogs();
    m_Viewport.Render(payload.FrameBufferID);
    m_ScenePanel.ShowSceneHierarchy();
    m_AssetBrowser.Render();
    m_PropertiesPanel.SetDropFilePath(m_AssetBrowser.GetDragFilePath());
    m_PropertiesPanel.SelectedProperties();


    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
