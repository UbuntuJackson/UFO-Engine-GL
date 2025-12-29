#include <stdexcept>
#include <string>
#include <SDL3/SDL.h>
#include "../ufo_maths/ufo_maths.h"
#include "actor.h"
#include "engine.h"
#include "camera.h"
#include "../../shapes/rectangle.h"
#include "sprite.h"
#include "../ufo_engine_studio/file_dialogue.h"
#include "../ufo_engine_studio/level_editor_tab.h"
#include "../ufo_engine_studio/editor.h"

Sprite::Sprite(std::string _key, olc::vf2d _position, olc::vf2d _offset, olc::vf2d _frame_size, olc::vf2d _scale, float _rotation, float _frame_index) :
key{_key},
Actor(_position),
offset{_offset},
frame_size{_frame_size},
scale{_scale},
rotation{_rotation},
current_frame_index{_frame_index}
{
    class_name = "Sprite";
    base_class_name = class_name;
}

void Sprite::OnSpawn(){
    //if(frame_size.x > engine->asset_manager.at(key).w) frame_size.x = engine->asset_manager.at(key).w;
    //if(frame_size.y > engine->asset_manager.at(key).h) frame_size.y = engine->asset_manager.at(key).h;
}

ufo::Rectangle
Sprite::GetRectangle(int _x, int _y, Vector2f _frame_size){
    ufo::Rectangle rect = ufo::Rectangle({(float)(_x * _frame_size.x), (float)(_y * _frame_size.y)}, _frame_size);
    return rect;
}

ufo::Rectangle
Sprite::GetFrameFromSpriteSheet(std::string _sprite_key, int _frame, Vector2f _frame_size){
    int fx = 1;
    int fy = 1;
    //To be fixed, clamp the values for height and width
    if(_frame_size.x > 0.0f && _frame_size.y > 0.0f &&
        _frame_size.x <= engine->asset_manager.textures.at(_sprite_key).width && frame_size.y <= engine->asset_manager.textures.at(_sprite_key).height){
        fx = (int)_frame % (engine->asset_manager.textures.at(_sprite_key).width/(int)_frame_size.x); //1 can only give me x = 0
        fy = (int)_frame / (engine->asset_manager.textures.at(_sprite_key).width/(int)_frame_size.x);
    }
    return GetRectangle(
        fx,
        fy,
        _frame_size); //1 can only give y = 1
}

void Sprite::OnDraw(ufo::Graphics* _graphics, Camera* _camera){

    ufo::Rectangle sample_rectangle = GetFrameFromSpriteSheet(key,current_frame_index,frame_size);
    _graphics->DrawPartialSprite(
        key,
        _camera->Transform(GetGlobalPosition()),
        /*size,*/
        offset,
        scale*_camera->scale,
        sample_rectangle.position,
        sample_rectangle.size,
        rotation,
        tint
    );
}

void Sprite::OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera){

}

void Sprite::OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){
    Actor::OnViewProperties(_level_editor_tab, _index);

    ImGui::InputFloat("offset.x",&offset.x);
    ImGui::InputFloat("offset.y",&offset.y);
    ImGui::InputFloat("frame_size.x",&frame_size.x);
    ImGui::InputFloat("frame_size.y",&frame_size.y);
    ImGui::InputFloat("scale.x",&scale.x);
    ImGui::InputFloat("scale.y",&scale.y);
    ImGui::InputFloat("rotation (degrees)",&rotation);
    ImGui::InputFloat("current_frame_index",&current_frame_index);

    ImGui::Separator();

    if(ImGui::Button("Add Texture")){
        SDL_ShowOpenFileDialog(&UFOEngineStudio::OnOpenTileset, _level_editor_tab, _level_editor_tab->engine->window, nullptr, 0, _level_editor_tab->editor->opened_directory_path.c_str(), false);
    }

    for(const auto& [name, texture] : engine->asset_manager.textures){
        ImGui::Image(
            (void*)(intptr_t)texture.id,
            ImVec2(16, 16),
            ImVec2(0,0),
            ImVec2(1,-1)
        );
    }

}

ufo::gc::JsonMap* Sprite::GetAsJson(ufo::GarbageCollector* _gc){
    Console::PrintLine("Does this even run?");

    ufo::gc::JsonMap* parent_class_as_json = Actor::GetAsJson(_gc);

    parent_class_as_json->map.emplace("key", _gc->New<ufo::gc::JsonString>(key));
    parent_class_as_json->map.emplace("offset_x", _gc->New<ufo::gc::JsonNumber>(offset.x));
    parent_class_as_json->map.emplace("offset_y", _gc->New<ufo::gc::JsonNumber>(offset.y));
    parent_class_as_json->map.emplace("frame_size_x", _gc->New<ufo::gc::JsonNumber>(frame_size.x));
    parent_class_as_json->map.emplace("frame_size_y", _gc->New<ufo::gc::JsonNumber>(frame_size.y));
    parent_class_as_json->map.emplace("scale_x", _gc->New<ufo::gc::JsonNumber>(scale.x));
    parent_class_as_json->map.emplace("scale_y", _gc->New<ufo::gc::JsonNumber>(scale.y));
    parent_class_as_json->map.emplace("rotation", _gc->New<ufo::gc::JsonNumber>(rotation));
    parent_class_as_json->map.emplace("frame_index", _gc->New<ufo::gc::JsonNumber>(current_frame_index));


    return parent_class_as_json;
}
