#include "../imgui/imgui.h"
#include "imgui_utils.h"
#include "../src/level.h"

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

void TextureOptions(ufo::Level* _level, const std::string _texture_name, ufo::Texture2D& _texture){
    if(_texture.is_savable){

        bool is_level_asset_dummy = _level->level_textures.count(_texture_name);

        const bool is_level_asset_result = is_level_asset_dummy;

        if(ImGui::Checkbox(std::string("Is Level Asset###IsLevelTextureCheckBox"+_texture_name).c_str(),&is_level_asset_dummy)){
            if(is_level_asset_result) _level->level_textures.erase(_texture_name);
            else _level->level_textures.insert(_texture_name);
        }
        ImGui::Checkbox(std::string("Is Global Asset###IsGlobalTextureCheckBox"+_texture_name).c_str(),&_texture.is_global_asset);

    }
}

}
