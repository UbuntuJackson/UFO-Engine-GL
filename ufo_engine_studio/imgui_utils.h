#pragma once
#include "../imgui/imgui.h"
#include "../src/texture_2d.h"
#include "level_editor_tab.h"
namespace ufo{
    class Level;
    class Engine;
}

namespace UFOEngineStudio{

class LevelEditorTab;

void PushStyleCompact();
void PopStyleCompact();

bool BeginWindow(const char* name, bool* p_open, ImGuiWindowFlags flags);
void EndWindow();
void PushStyleCloseButton();

void TextureOptions(ufo::Level* _level, const std::string _texture_name, ufo::Texture2D& _texture);

void TextureSavabilityAndAvailabilityDetails(ufo::Engine* _engine,ufo::Level* _level, const std::string _texture_name, ufo::Texture2D& _texture);

bool IsTextureAvailableInAssetBrowser(LevelEditorTab* _level_editor_tab, ufo::Level* _level, const std::string _texture_name, ufo::Texture2D& _texture);

}
