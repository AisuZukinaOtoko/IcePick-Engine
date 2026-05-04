#include "Toolbar.h"
#include "IcePickCoreMinimal.h"
#include "imgui-docking/imgui.h"


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
            bool& renderDebugPhysics = m_EngineAPI.QueryEngineRenderDebugPhysics();
            ImGui::MenuItem("Show Debug Physics", nullptr, &renderDebugPhysics);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Debug")) {
            if (ImGui::MenuItem("Reload Default Shader")) {
                m_EngineAPI.ReloadShaders();
            }

            //ImGui::SetNextItemShortcut(ImGuiMod_Alt | ImGuiKey_S);
            if (ImGui::MenuItem("Reload Scripts")) {
                m_EngineAPI.ReloadScripts();
                IP_LOG("Reloading entity scripts.");
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}