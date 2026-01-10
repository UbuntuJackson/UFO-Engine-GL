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

}
