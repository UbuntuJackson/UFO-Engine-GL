#include "../imgui/imgui.h"
#include "imgui_utils.h"

namespace UFOEngineStudio{

void PushStyleCompact()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::PushStyleVarY(ImGuiStyleVar_FramePadding, (float)(int)(style.FramePadding.y * 0.80f));
    ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, (float)(int)(style.ItemSpacing.y * 0.80f));
}

void PopStyleCompact()
{
    ImGui::PopStyleVar(2);
}

void PushStyleCloseButton(){

    //ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.6f, 0.6f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));

}

bool BeginWindow(const char* name, bool* p_open, ImGuiWindowFlags flags){

    PushStyleCloseButton();

    bool result = ImGui::Begin(name, p_open, flags);

    ImGui::PopStyleColor(3);
    //ImGui::PopStyleVar(1);

    return result;

}

void EndWindow(){

    ImGui::End();
}

}
