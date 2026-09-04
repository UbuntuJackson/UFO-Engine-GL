#include "../imgui/imgui.h"
#include "imgui_utils.h"
#include "../src/level.h"
#include "level_editor_tab.h"
#include "editor.h"
#include "../src/engine.h"
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

        bool is_level_asset_dummy = _level->local_textures.count(_texture_name);

        const bool is_level_asset_result = is_level_asset_dummy;

        if(ImGui::Checkbox(std::string("Is Level Asset###IsLevelTextureCheckBox"+_texture_name).c_str(),&is_level_asset_dummy)){
            if(is_level_asset_result) _level->local_textures.erase(_texture_name);
            else _level->local_textures.insert(_texture_name);
        }
        ImGui::Checkbox(std::string("Is Global Asset###IsGlobalTextureCheckBox"+_texture_name).c_str(),&_texture.is_global_asset);

    }
}

void TextureSavabilityAndAvailabilityDetails(ufo::Engine* _engine,ufo::Level* _level, const std::string _texture_name, ufo::Texture2D& _texture){
    std::string savable_status = _texture.is_savable ? "Savable" : "Not Savable";
    std::string locality_status = _texture.is_global_asset ? "Global" : "Local";

    const std::string status_text = "Status: "+savable_status+", "+locality_status;

    ImGui::Text("%s",status_text.c_str());

    if(ImGui::Button(std::string("Create Local Texture###"+_texture_name).c_str())){
        const std::string local_texture_name = _texture_name+"_local"+std::to_string(_engine->local_asset_counter++);

        if(!_engine->asset_manager.textures.count(local_texture_name)){
            _engine->asset_manager.LoadTexture(_engine->editor.opened_directory_path,_texture.path_relative_to_project, local_texture_name, true);

            if(!_engine->asset_manager.textures.count(local_texture_name)) Console::PrintLine(__UFO_PRETTY_FUNCTION__, "Error, could not create local texture");
            else{
                _level->local_textures.insert(local_texture_name);
                _engine->asset_manager.textures.at(local_texture_name).is_global_asset = false;
                _engine->asset_manager.textures.at(local_texture_name).is_savable = true;
            }
        }
        else Console::PrintLine(__UFO_PRETTY_FUNCTION__, "Error, local texture with name",local_texture_name,"already exists");

    }
}

bool IsTextureAvailableInAssetBrowser(LevelEditorTab* _level_editor_tab, ufo::Level* _level, const std::string _texture_name, ufo::Texture2D& _texture){
    if(!_texture.is_global_asset){
        if(!_level->local_textures.count(_texture_name)) return false;
    }

    if(_level_editor_tab->asset_view_mode != UFOEngineStudio::LevelEditorTab::ALL){
        if(_level_editor_tab->asset_view_mode == UFOEngineStudio::LevelEditorTab::LOCAL){
            if(!_level->local_textures.count(_texture_name)) return false;
        }
        if(_level_editor_tab->asset_view_mode == UFOEngineStudio::LevelEditorTab::GLOBAL){
            if(!_texture.is_global_asset) return false;
        }
    }

    return true;
}

/*void TextureTab(UFOEngineStudio::LevelEditorTab* _level_editor_tab, ufo::Engine* _engine, ufo::Level* _level, std::map<std::string, std::string&> _keys_to_set){
    if(ImGui::BeginTabItem("Textures")){

        if(ImGui::Button("[+] Add Texture")){
            SDL_ShowOpenFileDialog(&UFOEngineStudio::OnOpenTexture, _level_editor_tab, _engine->window, UFOEngineStudio::global_texture_filters, 2, _level_editor_tab->editor->opened_directory_path.c_str(), true);
        }

        ImGui::SameLine();

        std::string preview_value = "";

        switch(_level_editor_tab->asset_view_mode){
            case UFOEngineStudio::LevelEditorTab::AssetViewMode::GLOBAL:{
                preview_value = "Global Assets";
                break;
            }
            case UFOEngineStudio::LevelEditorTab::AssetViewMode::LOCAL:{
                preview_value = "Local Assets";
                break;
            }
            case UFOEngineStudio::LevelEditorTab::AssetViewMode::ALL:{
                preview_value = "All Assets";
                break;
            }
        }

        if(ImGui::BeginCombo("View###ViewGlobalOrLocalAsset", preview_value.c_str())){

            if(ImGui::Selectable("Global Assets")){
                _level_editor_tab->asset_view_mode = UFOEngineStudio::LevelEditorTab::AssetViewMode::GLOBAL;
            }
            if(ImGui::Selectable("Local Assets")){
                _level_editor_tab->asset_view_mode = UFOEngineStudio::LevelEditorTab::AssetViewMode::LOCAL;
            }
            if(ImGui::Selectable("All Assets")){
                _level_editor_tab->asset_view_mode = UFOEngineStudio::LevelEditorTab::AssetViewMode::ALL;
            }

            ImGui::EndCombo();
        }

        if(ImGui::InputText("Search###SearchAssetBrowser", &_level_editor_tab->asset_browser_search)){

        }

        ImGui::Separator();

        if(ImGui::BeginChild("MyAssetsChildWindow")){

            bool texture_was_erased = false;
            std::string name_of_erased_texture = "";

            std::vector<std::string> texture_names;
            for(const auto& [name, texture] : _engine->asset_manager.textures){
                bool search_is_in_word = false;

                for(int c = 0; c < (int)name.size(); c++){
                    bool found_match_from_this_character = true;

                    for(int d = 0; d < (int)_level_editor_tab->asset_browser_search.size(); d++){
                        if(c+d > (int)name.size()-1) continue;

                        if(_level_editor_tab->asset_browser_search[d]!=name[c+d]){
                            found_match_from_this_character = false;
                        }
                    }

                    if(found_match_from_this_character) search_is_in_word = true;
                }

                if(search_is_in_word) texture_names.push_back(name);
            }
            std::sort(texture_names.begin(), texture_names.end(), [](const std::string& _a,const std::string& _b){
                return _a<_b;
            });

            for(const std::string& name : texture_names){

                auto& texture = _engine->asset_manager.textures.at(name);

                if(_level_editor_tab->asset_view_mode != UFOEngineStudio::LevelEditorTab::ALL){
                    if(_level_editor_tab->asset_view_mode == UFOEngineStudio::LevelEditorTab::LOCAL){
                        if(!level->level_textures.count(name)) continue;
                    }
                    if(_level_editor_tab->asset_view_mode == UFOEngineStudio::LevelEditorTab::GLOBAL){
                        if(!texture.is_global_asset) continue;
                    }
                }

                float w = (float)texture.width;
                float h = (float)texture.height;

                bool view_asset_details = ImGui::CollapsingHeader(std::string("###view_asset_details"+name).c_str(), nullptr, ImGuiTreeNodeFlags_SpanTextWidth);

                ImGui::SameLine();

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));
                ImGui::ImageButton(name.c_str(),
                    (void*)(intptr_t)texture.id,
                    ImVec2(32.0f*w/h, 32.0f),
                    ImVec2(0,0),
                    ImVec2(1,1),
                    ImVec4(0,0,0,1)
                );
                ImGui::PopStyleVar();

                if(ImGui::IsItemHovered()) ImGui::SetTooltip(name.c_str(), "%s");

                if(view_asset_details){
                    if(ImGui::Button(std::string("Unload Texture###UnloadTexture"+name).c_str())){
                        name_of_erased_texture = name;
                        texture_was_erased = true;
                    }
                    ImGui::SameLine();
                    if(ImGui::Button(std::string("Assign Texture to Current Sprite###AddCostume"+name).c_str())){
                        texture_key = name;
                        frame_size = Vector2f(w,h);
                        number_of_frames = 1;

                    }
                    ImGui::Text(std::string("width: " + std::to_string(w) + " height: "+std::to_string(h)).c_str(),"%s");
                    ImGui::Text(("name: "+name).c_str(),"%s");
                    ImGui::Text(texture.is_savable ? "Status: Savable" : "Status: Not Savable");
                    UFOEngineStudio::TextureOptions(_level, name, texture);
                }

            }

            if(texture_was_erased && name_of_erased_texture != "placeholder_icon"){
                _engine->asset_manager.textures.at(name_of_erased_texture).Delete();
                _engine->asset_manager.textures.erase(name_of_erased_texture);
                for(const auto& loaded_level : _engine->loaded_levels_for_editor) loaded_level->ResourcesEdited();

                if(texture_key == name_of_erased_texture) texture_key = "placeholder_icon";

            }

            ImGui::EndChild();

        }

        ImGui::EndTabItem();
    }
}*/

}
