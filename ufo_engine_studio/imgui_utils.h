#pragma once
#include "../imgui/imgui.h"
#include "../src/texture_2d.h"
namespace ufo{
    class Level;
}

namespace UFOEngineStudio{

void PushStyleCompact();
void PopStyleCompact();

bool BeginWindow(const char* name, bool* p_open, ImGuiWindowFlags flags);
void EndWindow();
void PushStyleCloseButton();

void TextureOptions(ufo::Level* _level, const std::string _texture_name, ufo::Texture2D& _texture);

}
