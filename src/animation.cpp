#include <exception>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include "../ufo_maths/ufo_maths.h"
#include "actor.h"
#include "animation.h"
#include "console.h"
#include "sprite.h"
#include "../ufo_garbage_collector/garbage_collector.h"
#include "../ufo_garbage_collector/gc_json.h"
#include "texture_2d.h"
#include "engine.h"
#include "ufo_macros.h"
#include "sprite_utils.h"

#ifdef UFO_ENGINE_STUDIO
#include "../ufo_engine_studio/file_dialogue.h"
#include "../ufo_engine_studio/editor.h"
#include "../ufo_engine_studio/level_editor_tab.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "../ufo_engine_studio/imgui_utils.h"
#endif //UFO_ENGINE_STUDIO

namespace ufo{

Animation::Animation(Vector2f _local_position) : Sprite(_local_position, "placeholder_icon", Vector2f(0.0f, 0.0f), Vector2f(16.0f, 16.0f), Vector2f(1.0f, 1.0f), 0.0f, 0.0f){

    class_name = "ufo::Animation";
    base_class_name = class_name;

}

void Animation::SetCostume(const std::string& _configuration_key){
    // If the key does not exist in the asset manager, then this code will just fail silently. That's pretty bad.

    if(costumes.count(_configuration_key)){
        Costume& costume = costumes.at(_configuration_key);
        key = costume.key;
        offset = costume.offset;
        frame_size = costume.frame_size;
        scale = costume.scale;
        rotation = costume.rotation;
        current_frame_index = costume.frame_index;
        animation_speed = costume.animation_speed;
    }
    else{
        Console::PrintLine(__UFO_PRETTY_FUNCTION__, "Could not find costume with key:", _configuration_key);

    }

    if(!engine->asset_manager.textures.count(_configuration_key)){
        Console::PrintLine(__UFO_PRETTY_FUNCTION__, "Could not find texture with key:", _configuration_key);
        costumes.erase(_configuration_key);
        SetCostume("placeholder_icon");
        return;
    }

    ufo::Texture2D& ref_texture = engine->asset_manager.textures.at(key);
    number_of_frames = (float)((unsigned int)ref_texture.width/(unsigned int)frame_size.x * (unsigned int)ref_texture.height/(unsigned int)frame_size.y);

    if(frame_size.x == 0.0f || frame_size.y == 0.0f){
        Console::PrintLine(__UFO_PRETTY_FUNCTION__,"frame_size has invalid proportions:", frame_size.x, frame_size.y);
        SetCostume("placeholder_icon");
    }
}

void Animation::AddCostume(std::string _key, olc::vf2d _local_position, olc::vf2d _offset, olc::vf2d _frame_size, olc::vf2d _scale, float _rotation, float _frame_index, float _animation_speed){
    Costume costume = {
        _key,
        _local_position,
        _offset,
        _frame_size,
        _scale,
        _rotation,
        _frame_index,
        _animation_speed
    };

    if(costume.frame_size.x == 0.0f || costume.frame_size.y == 0.0f){
        Console::PrintLine(__UFO_PRETTY_FUNCTION__,"frame_size has invalid proportions:", costume.frame_size.x, costume.frame_size.y);
        return;
    }

    costumes.emplace(_key, costume);

}

void Animation::OnSpawn(){

    AddCostume("placeholder_icon", Vector2f(0.0f,0.0f), Vector2f(0.0f,0.0f), Vector2f(32.0f,32.0f), Vector2f(1.0f, 1.0f), 0.0f, 0.0f, 0.0f);
    SetCostume(key);

}

void Animation::OnUpdate(float _delta_time){
    if(preview){
        current_frame_index += animation_speed * _delta_time;
        //current_frame_index = ufoMaths::Wrap(frame_counter, 0.0f, number_of_frames);
    }
}

ufo::gc::JsonMap* Animation::GetAsJson(ufo::GarbageCollector* _gc){
    Console::PrintLine("Does this even run?");

    ufo::gc::JsonMap* parent_class_as_json = Actor::GetAsJson(_gc);

    if(import_mode == CUSTOM_CLASS) return parent_class_as_json;

    auto j_costumes = _gc->New<ufo::gc::JsonArray>();

    for(const auto& [k,v] : costumes){

        auto j_costume = _gc->New<ufo::gc::JsonMap>();

        j_costume->map.emplace("key", _gc->New<ufo::gc::JsonString>(v.key));
        j_costume->map.emplace("offset_x", _gc->New<ufo::gc::JsonNumber>(v.offset.x));
        j_costume->map.emplace("offset_y", _gc->New<ufo::gc::JsonNumber>(v.offset.y));
        j_costume->map.emplace("frame_size_x", _gc->New<ufo::gc::JsonNumber>(v.frame_size.x));
        j_costume->map.emplace("frame_size_y", _gc->New<ufo::gc::JsonNumber>(v.frame_size.y));
        j_costume->map.emplace("scale_x", _gc->New<ufo::gc::JsonNumber>(v.scale.x));
        j_costume->map.emplace("scale_y", _gc->New<ufo::gc::JsonNumber>(v.scale.y));
        j_costume->map.emplace("rotation", _gc->New<ufo::gc::JsonNumber>(v.rotation));
        j_costume->map.emplace("frame_index", _gc->New<ufo::gc::JsonNumber>(v.frame_index));
        j_costume->map.emplace("animation_speed", _gc->New<ufo::gc::JsonNumber>(v.animation_speed));

        j_costumes->array.push_back(j_costume);
    }

    parent_class_as_json->map.emplace("corner_rounding", _gc->New<ufo::gc::JsonNumber>(corner_rounding));
    parent_class_as_json->map.emplace("shader_key",_gc->New<ufo::gc::JsonString>(shader_key));

    ufo::gc::JsonArray* j_colour = _gc->New<ufo::gc::JsonArray>();
    j_colour->array.push_back(_gc->New<ufo::gc::JsonNumber>(tint.r));
    j_colour->array.push_back(_gc->New<ufo::gc::JsonNumber>(tint.g));
    j_colour->array.push_back(_gc->New<ufo::gc::JsonNumber>(tint.b));
    j_colour->array.push_back(_gc->New<ufo::gc::JsonNumber>(tint.a));

    parent_class_as_json->map.emplace("tint", j_colour);

    parent_class_as_json->map.emplace("costumes",j_costumes);
    parent_class_as_json->map.emplace("current_costume",_gc->New<ufo::gc::JsonString>(key));
    parent_class_as_json->map.emplace("preview",_gc->New<ufo::gc::JsonNumber>(preview));


    return parent_class_as_json;
}

void Animation::OnLoadDefaultProperties(ufo::gc::JsonMap* _json){

    //if(import_mode == Actor::ImportModes::UNWRAPPED){

    if(!_json->map.count("costumes")){
        if(import_mode == ImportModes::BUILT_IN_CLASS) Console::PrintLine(GetInfo(), __UFO_PRETTY_FUNCTION__, "Error, costumes don't exist");
        else return;
    }

    for(const auto& j_costume : _json->map.at("costumes")->AsArray()){
        Animation::Costume costume;
        costume.key = j_costume->AsMap().at("key")->AsString();
        costume.offset.x = j_costume->AsMap().at("offset_x")->AsFloat();
        costume.offset.y = j_costume->AsMap().at("offset_y")->AsFloat();
        costume.frame_size.x = j_costume->AsMap().at("frame_size_x")->AsFloat();
        costume.frame_size.y = j_costume->AsMap().at("frame_size_y")->AsFloat();
        costume.scale.x = j_costume->AsMap().at("scale_x")->AsFloat();
        costume.scale.y = j_costume->AsMap().at("scale_y")->AsFloat();
        costume.rotation = j_costume->AsMap().at("rotation")->AsFloat();
        costume.frame_index = j_costume->AsMap().at("frame_index")->AsFloat();
        costume.animation_speed = j_costume->AsMap().at("animation_speed")->AsFloat();

        costumes.emplace(costume.key, costume);
    }

    _json->TryToGetValueAsString("current_costume", key, GetInfo() + " " + __UFO_PRETTY_FUNCTION__);

    float f_preview = 0.0f;
    _json->TryToGetValueAsFloat("preview", f_preview, GetInfo() + " " + __UFO_PRETTY_FUNCTION__);

    preview = (float)f_preview;

    _json->TryToGetValueAsString("shader_key", shader_key, GetInfo() + " " + __UFO_PRETTY_FUNCTION__);

    std::vector<gc::Json *> j_colour;
    _json->TryToGetValueAsArray("tint", j_colour, GetInfo() + " " + __UFO_PRETTY_FUNCTION__);
    if((int)j_colour.size() == 4){
        float red = j_colour[0]->AsFloat();
        float green = j_colour[1]->AsFloat();
        float blue = j_colour[2]->AsFloat();
        float alpha = j_colour[3]->AsFloat();
        tint = ufo::Colour(red, green, blue, alpha);
#ifdef UFO_ENGINE_STUDIO
        im_colour = ImVec4(red/255.0f,green/255.0f,blue/255.0f,alpha/255.0f);
#endif
    }

    _json->TryToGetValueAsFloat("corner_rounding", corner_rounding, GetInfo() + " " + __UFO_PRETTY_FUNCTION__);

    //}
}

#ifdef UFO_ENGINE_STUDIO

void Animation::OnUtiliseAssetManager(UFOEngineStudio::LevelEditorTab* _level_editor_tab){

    if(ImGui::BeginTabItem("MyAssets")){

        if(ImGui::Button("[+] Add Texture")){
            SDL_ShowOpenFileDialog(&UFOEngineStudio::OnOpenTexture, _level_editor_tab, engine->window, UFOEngineStudio::global_texture_filters, 2, _level_editor_tab->editor->opened_directory_path.c_str(), true);
        }

        if(ImGui::InputText("Search###SearchAssetBrowser", &_level_editor_tab->asset_browser_search)){

        }

        ImGui::Separator();

        if(ImGui::BeginChild("MyAssetsChildWindow")){

        bool texture_was_erased = false;
        std::string name_of_erased_texture = "";

        std::vector<std::string> texture_names;
        for(const auto& [name, texture] : engine->asset_manager.textures){
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

            auto& texture = engine->asset_manager.textures.at(name);

            if(!texture.is_global_asset && !level->level_textures.count(name)) continue;

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
                if(ImGui::Button(std::string("Add Texture as Costume###AddCostume"+name).c_str())){
                    AddCostume(name, Vector2f(0.0f,0.0f), Vector2f(0.0f,0.0f), Vector2f(w,h), Vector2f(1.0f, 1.0f), 0.0f, 0.0f, 0.0f);
                    SetCostume(name);
                }
                ImGui::Text(std::string("width: " + std::to_string(w) + " height: "+std::to_string(h)).c_str(),"%s");
                ImGui::Text(("name:"+name).c_str(),"%s");
                ImGui::Text(texture.is_savable ? "Status: Savable" : "Status: Savable");
                UFOEngineStudio::TextureOptions(level, name, texture);
            }

        }

        if(texture_was_erased && name_of_erased_texture != "placeholder_icon"){
            engine->asset_manager.textures.at(name_of_erased_texture).Delete();
            engine->asset_manager.textures.erase(name_of_erased_texture);

            if(key == name_of_erased_texture) SetCostume("placeholder_icon");

        }
            ImGui::EndChild();

        }

        ImGui::EndTabItem();
    }

    if(ImGui::BeginTabItem("Shaders")){

        if(ImGui::Button("[+] Add Shader")){
            SDL_ShowOpenFolderDialog(&UFOEngineStudio::OnOpenShader, _level_editor_tab, engine->window, _level_editor_tab->editor->opened_directory_path.c_str(), true);
        }

        if(ImGui::InputText("Search###SearchShaders", &_level_editor_tab->asset_browser_search)){

        }

        if(ImGui::BeginChild("MyShaders")){

            bool shader_was_erased = false;
            std::string name_of_erased_shader = "";

            std::vector<std::string> shader_names;
            for(const auto& [name, shader] : engine->asset_manager.shaders){
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

                if(search_is_in_word) shader_names.push_back(name);
            }
            std::sort(shader_names.begin(), shader_names.end(), [](const std::string& _a,const std::string& _b){
                return _a<_b;
            });

            for(const std::string& name : shader_names){

                bool view_asset_details = ImGui::CollapsingHeader(std::string(("name:"+name)+"###view_asset_details"+name).c_str(), nullptr, ImGuiTreeNodeFlags_SpanTextWidth);

                if(ImGui::IsItemHovered()) ImGui::SetTooltip(name.c_str(), "%s");

                if(view_asset_details){
                    if(ImGui::Button(std::string("Unload Shader###UnloadShader"+name).c_str())){
                        name_of_erased_shader = name;
                        shader_was_erased = true;
                    }
                    ImGui::SameLine();
                    if(ImGui::Button(std::string("Assign Shader to Current Sprite###AddCostume"+name).c_str())){
                        shader_key = name;

                    }

                    ImGui::Text(("name:"+name).c_str(),"%s");
                }

            }

            if(shader_was_erased && name_of_erased_shader != "partial_sprite_shader"){
                engine->asset_manager.shaders.at(name_of_erased_shader).Delete();
                engine->asset_manager.shaders.erase(name_of_erased_shader);
                for(const auto& loaded_level : engine->loaded_levels_for_editor) loaded_level->ResourcesEdited();

                if(shader_key == name_of_erased_shader) shader_key = "partial_sprite_shader";

            }

            ImGui::EndChild();
        }


        ImGui::EndTabItem();

    }
}

void Animation::OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){

    Actor::OnViewProperties(_level_editor_tab, _index);

    if(import_mode == ImportModes::BUILT_IN_CLASS){

        std::string removed_costume_key;

        if(ImGui::Button(preview ? "||" : ">")){
            preview = !preview;
        }

        std::vector<std::string> costumes_to_erase;

        for(const auto& [k,v] : costumes){
            if(!engine->asset_manager.textures.count(v.key)) costumes_to_erase.push_back(k);
        }

        for(const std::string& costume_name : costumes_to_erase) costumes.erase(costume_name);

        //Actor::OnViewProperties(_level_editor_tab, _index);
        if(ImGui::BeginCombo(std::string("Costume###Costume"+std::to_string(editor_id)).c_str(), key.c_str())){

            for(const auto& [k,v] : costumes){
                bool is_selected = (key == k);

                bool selectable_pressed = ImGui::Selectable(k.c_str(), &is_selected);

                if(selectable_pressed){
                    SetCostume(k);
                }

                if(is_selected){
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        //This could be faulty. Better to have assets marked removable and non-removable.
        if(key != "placeholder_icon"){
            ImGui::SameLine();

            if(ImGui::Button(std::string("Remove###RemoveAnimationDialogue"+std::to_string(editor_id)).c_str())){
                costumes.erase(key);
                SetCostume("placeholder_icon");
            }
        }

        if(ImGui::InputFloat("offset.x",&costumes.at(key).offset.x)) offset.x = costumes.at(key).offset.x;
        if(ImGui::InputFloat("offset.y",&costumes.at(key).offset.y)) offset.y = costumes.at(key).offset.y;
        if(ImGui::InputFloat("frame_size.x",&costumes.at(key).frame_size.x)) frame_size.x = costumes.at(key).frame_size.x;
        if(ImGui::InputFloat("frame_size.y",&costumes.at(key).frame_size.y)) frame_size.y = costumes.at(key).frame_size.y;
        if(ImGui::InputFloat("scale.x",&costumes.at(key).scale.x)) scale.x = costumes.at(key).scale.x;
        if(ImGui::InputFloat("scale.y",&costumes.at(key).scale.y)) scale.y = costumes.at(key).scale.y;
        if(ImGui::InputFloat("rotation (degrees)",&costumes.at(key).rotation)) rotation = costumes.at(key).rotation;
        if(ImGui::InputFloat("current_frame_index",&costumes.at(key).frame_index)) current_frame_index = costumes.at(key).frame_index;
        if(ImGui::InputFloat("animation_speed",&costumes.at(key).animation_speed)) animation_speed = costumes.at(key).animation_speed;
        ImGui::InputFloat("corner_rounding",&corner_rounding);

        ImVec4 start_colour =  ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

        if(ImGui::ColorPicker4(std::string("MyColor##4"+std::to_string(editor_id)).c_str(), (float*)&im_colour, ImGuiColorEditFlags_AlphaBar, (float*)&start_colour)){
            tint = ufo::Colour(im_colour.x*255.0f, im_colour.y*255.0f, im_colour.z*255.0f, im_colour.w*255.0f);
            Console::PrintLine(im_colour.x*255.0f, im_colour.y*255.0f, im_colour.z*255.0f, im_colour.w*255.0f);
        }

        ImGui::Text("Shader: %s", shader_key.c_str());

    }

}

void Animation::OnResourcesEdited(){
    if(!engine->asset_manager.textures.count(key)){
        costumes.erase(key);
        key = "placeholder_icon";
        SetCostume(key);
    }
    if(!engine->asset_manager.shaders.count(shader_key)){
        shader_key = "partial_sprite_shader";
    }
}

#endif //UFO_ENGINE_STUDIO

}
