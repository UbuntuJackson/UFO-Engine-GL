#include <exception>
#include <unordered_map>
#include <vector>
#include "../ufo_maths/ufo_maths.h"
#include "actor.h"
#include "animation.h"
#include "sprite.h"
#include "../ufo_engine_studio/file_dialogue.h"
#include "../ufo_engine_studio/editor.h"
#include "../ufo_engine_studio/level_editor_tab.h"
#include "../ufo_garbage_collector/garbage_collector.h"
#include "../ufo_garbage_collector/gc_json.h"
#include "texture_2d.h"

Animation::Animation(Vector2f _local_position) : Sprite("placeholder_icon", _local_position, Vector2f(0.0f, 0.0f), Vector2f(16.0f, 16.0f), Vector2f(1.0f, 1.0f), 0.0f, 0.0f){

    class_name = "Animation";
    base_class_name = class_name;

}

void Animation::SetCostume(const std::string& _configuration_key){
    Costume& costume = costumes.at(_configuration_key);
    key = costume.key;
    local_position = costume.local_position;
    offset = costume.offset;
    frame_size = costume.frame_size;
    scale = costume.scale;
    rotation = costume.rotation;
    current_frame_index = costume.frame_index;
    animation_speed = costume.animation_speed;

    if(!engine->asset_manager.textures.count(_configuration_key)){
        Console::PrintLine("[UFO-Engine] Animation::SetCostume: Could not find texture with key:", _configuration_key);
        costumes.erase(_configuration_key);
        SetCostume("placeholder_icon");
        return;
    }

    ufo::Texture2D& ref_texture = engine->asset_manager.textures.at(key);
    number_of_frames = (float)(ref_texture.width/(unsigned int)frame_size.x * ref_texture.height/(unsigned int)frame_size.y);

    if(frame_size.x == 0.0f || frame_size.y == 0.0f){
        Console::PrintLine("[UFO-Engine] Animation::SetCostume: frame_size has invalid proportions:", frame_size.x, frame_size.y);
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
        Console::PrintLine("[UFO-Engine] Animation::SetCostume: frame_size has invalid proportions:", costume.frame_size.x, costume.frame_size.y);
        return;
    }

    costumes.emplace(_key, costume);

}

void Animation::OnSpawn(){

    AddCostume("placeholder_icon", Vector2f(0.0f,0.0f), Vector2f(0.0f,0.0f), Vector2f(32.0f,32.0f), Vector2f(1.0f, 1.0f), 0.0f, 0.0f, 0.0f);
    SetCostume(key);

}

void Animation::OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){

    std::string removed_costume_key;
    bool did_remove_costume = false;

    if(ImGui::Button(preview ? "||" : ">")){
        preview = !preview;
    }

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

    if(ImGui::InputFloat("position.x",&costumes.at(key).local_position.x)) local_position.x = costumes.at(key).local_position.x;
    if(ImGui::InputFloat("position.y",&costumes.at(key).local_position.y)) local_position.y = costumes.at(key).local_position.y;
    if(ImGui::InputFloat("offset.x",&costumes.at(key).offset.x)) offset.x = costumes.at(key).offset.x;
    if(ImGui::InputFloat("offset.y",&costumes.at(key).offset.y)) offset.y = costumes.at(key).offset.y;
    if(ImGui::InputFloat("frame_size.x",&costumes.at(key).frame_size.x)) frame_size.x = costumes.at(key).frame_size.x;
    if(ImGui::InputFloat("frame_size.y",&costumes.at(key).frame_size.y)) frame_size.y = costumes.at(key).frame_size.y;
    if(ImGui::InputFloat("scale.x",&costumes.at(key).scale.x)) scale.x = costumes.at(key).scale.x;
    if(ImGui::InputFloat("scale.y",&costumes.at(key).scale.y)) scale.y = costumes.at(key).scale.y;
    if(ImGui::InputFloat("rotation (degrees)",&costumes.at(key).rotation)) rotation = costumes.at(key).rotation;
    if(ImGui::InputFloat("current_frame_index",&costumes.at(key).frame_index)) current_frame_index = costumes.at(key).frame_index;
    if(ImGui::InputFloat("animation_speed",&costumes.at(key).animation_speed)) animation_speed = costumes.at(key).animation_speed;

    ImGui::Separator();

    if(ImGui::Button("Add Texture")){
        SDL_ShowOpenFileDialog(&UFOEngineStudio::OnOpenTexture, _level_editor_tab, _level_editor_tab->engine->window, nullptr, 0, _level_editor_tab->editor->opened_directory_path.c_str(), false);
    }

    bool texture_was_erased = false;
    std::string name_of_erased_texture = "";

    for(const auto& [name, texture] : engine->asset_manager.textures){
        ImGui::Text("%s",name.c_str());
        ImGui::Image(
            (void*)(intptr_t)texture.id,
            ImVec2(16, 16),
            ImVec2(0,0),
            ImVec2(1,1)
        );
        ImGui::SameLine();
        if(ImGui::Button(std::string("Add Texture as Costume###AddCostume"+name).c_str())){
            float w = (float)engine->asset_manager.textures.at(name).width;
            float h = (float)engine->asset_manager.textures.at(name).height;
            AddCostume(name, Vector2f(0.0f,0.0f), Vector2f(0.0f,0.0f), Vector2f(w,h), Vector2f(1.0f, 1.0f), 0.0f, 0.0f, 0.0f);
            SetCostume(name);
        }
        ImGui::SameLine();
        if(ImGui::Button(std::string("Unload###UnloadCostume"+name).c_str())){
            name_of_erased_texture = name;
            texture_was_erased = true;
        }
    }

    if(texture_was_erased && name_of_erased_texture != "placeholder_icon"){

        engine->asset_manager.textures.at(name_of_erased_texture).Delete();
        engine->asset_manager.textures.erase(name_of_erased_texture);

        if(key == name_of_erased_texture) SetCostume("placeholder_icon");
    }

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

    auto j_costumes = _gc->New<ufo::gc::JsonArray>();

    for(const auto& [k,v] : costumes){

        auto j_costume = _gc->New<ufo::gc::JsonMap>();

        j_costume->map.emplace("key", _gc->New<ufo::gc::JsonString>(v.key));
        j_costume->map.emplace("local_position_x", _gc->New<ufo::gc::JsonNumber>(v.local_position.x));
        j_costume->map.emplace("local_position_y", _gc->New<ufo::gc::JsonNumber>(v.local_position.y));
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

    parent_class_as_json->map.emplace("costumes",j_costumes);
    parent_class_as_json->map.emplace("current_costume",_gc->New<ufo::gc::JsonString>(key));
    parent_class_as_json->map.emplace("preview",_gc->New<ufo::gc::JsonNumber>(preview));


    return parent_class_as_json;
}
