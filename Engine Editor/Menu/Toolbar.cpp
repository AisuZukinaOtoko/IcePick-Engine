#include "Toolbar.h"
#include "../../src/IcePickCoreMinimal.h"


Toolbar::Toolbar(IcePick::EngineAPI engineAPI) :
    m_EngineAPI(engineAPI) 
{
    
}

void Toolbar::Render() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open")) { 
                
            }
            if (ImGui::MenuItem("Save")) {
                
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Debug")) {
            if (ImGui::MenuItem("Reload Shaders")) {
                m_EngineAPI.ReloadShaders();
            }

            ImGui::SetNextItemShortcut(ImGuiMod_Alt | ImGuiKey_S);
            if (ImGui::MenuItem("Reload Scripts (Alt+S)")) {
                m_EngineAPI.ReloadScripts();
                IP_LOG("Reloading entity scripts.");
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}