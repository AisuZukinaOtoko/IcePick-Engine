#include "Toolbar.h"
#include "../../src/IcePickCoreMinimal.h"

void Toolbar::Render() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open")) { 
                
            }
            if (ImGui::MenuItem("Save")) {
                
            }
            ImGui::EndMenu();
        }
        /*if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo")) {}
            if (ImGui::MenuItem("Redo")) {}
            ImGui::EndMenu();
        }*/
        ImGui::EndMainMenuBar();
    }
}