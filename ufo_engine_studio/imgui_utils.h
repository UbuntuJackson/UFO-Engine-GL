#pragma once
#include "../imgui/imgui.h"

namespace UFOEngineStudio{

void PushStyleCompact();
void PopStyleCompact();

bool BeginWindow(const char* name, bool* p_open, ImGuiWindowFlags flags);
void EndWindow();
void PushStyleCloseButton();

}
