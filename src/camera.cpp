#include "../ufo_maths/ufo_maths.h"
#include "actor.h"
#include "camera.h"
#include "level.h"
#include "engine.h"
#include <garbage_collector.h>
#include <gc_json.h>

#ifdef UFO_ENGINE_STUDIO
#include "../ufo_engine_studio/level_editor_tab.h"
#include "../imgui/imgui.h"
#endif

namespace ufo{

Camera::Camera(olc::vf2d _position):
Actor(_position),
scale{1.0f}
{

}

void Camera::OnSpawn(){
    view = Bounds{0.0f,(float)engine->width, 0.0f, (float)engine->height};
    world = Bounds{0.0f,level->size.x, 0.0f ,level->size.y};

    if(!engine->in_editor) level->active_camera_handles.push_back(this);
    class_name = "ufo::Camera";
    base_class_name = class_name;

    Console::PrintLine("Camera Spawned", editor_name);
}

void
Camera::HandleUpdate(){ //Updates as a handle.

    if(clamp){
        int scaled_width_half = (1.0f/scale)*viewport.size.x/2.0f; //bigger scale -> distance on screen smaller -> less world-space needed to clamp.
        int scaled_height_half = (1.0f/scale)*viewport.size.y/2.0f;

        if(GetGlobalPosition().x < world.x0 + scaled_width_half){
            local_position.x -= (GetGlobalPosition().x - (world.x0 + scaled_width_half));

        }
        else if(GetGlobalPosition().x > world.x1 - scaled_width_half){
            local_position.x -= (GetGlobalPosition().x - (world.x1 - scaled_width_half));

        }

        if(GetGlobalPosition().y < world.y0 + scaled_height_half){
            local_position.y -= (GetGlobalPosition().y - (world.y0 + scaled_height_half));

        }
        else if(GetGlobalPosition().y > world.y1 - scaled_height_half){
            local_position.y -= (GetGlobalPosition().y - (world.y1 - scaled_height_half));

        }

    }
}

void Camera::OnKilled(){
    for(int c = level->active_camera_handles.size()-1; c != -1; c--){
        if(level->active_camera_handles[c] == this) level->active_camera_handles.erase(level->active_camera_handles.begin()+c);
    }
}

olc::vf2d
Camera::Transform(const olc::vf2d& _position){ //position is supposed to be in the middle, everything else is drawn in the middle

    int screen_position_x = ((_position.x-GetGlobalPosition().x)*scale) + viewport.size.x/2.0f; //subtract with unscaled view to centre
    int screen_position_y = ((_position.y-GetGlobalPosition().y)*scale) + viewport.size.y/2.0f; //subtract with unscaled view to centre
    return olc::vf2d(screen_position_x, screen_position_y); //im not calculating screen_position_y here, just pretend I did this calculation for that too, haha
}

olc::vf2d
Camera::TransformScreenToWorld(const olc::vf2d& _screen_position){ //position is supposed to be in the middle, everything else is drawn in the middle

    /*if(clamp){ //for clamping x in this case, the same applies for y
        int scaled_width_half = (1.0f/scale)*world.GetWidthHalf(); //bigger scale -> distance on screen smaller -> less world-space needed to clamp.

        position.x = std::max(world.x0 + scaled_width_half, position.x - scaled_width_half); //world-wise this is the middle of the screen.
        position.x = std::min(world.x1 - scaled_width_half, position.x + scaled_width_half); //will subtract -view.GetWidthHalf() from here.
    }*/
    olc::vf2d world_position = {0.0f, 0.0f};
    world_position.x = (_screen_position.x - viewport.size.x/2.0f)/scale + GetGlobalPosition().x; //subtract with unscaled view to centre
    world_position.y = (_screen_position.y - viewport.size.y/2.0f)/scale + GetGlobalPosition().y; //subtract with unscaled view to centre
    return world_position; //im not calculating screen_position_y here, just pretend I did this calculation for that too, haha
}

bool
Camera::IsOnScreen(olc::vf2d _position, olc::vf2d _offset){
    return ufo::Maths::RectangleVsPoint(GetOnScreenRectangleInWorld(_offset),_position);
}

ufo::Rectangle
Camera::GetOnScreenRectangleInWorld(olc::vf2d _offset){
    float x0 = (GetGlobalPosition().x-(viewport.size.x/2.0f)/scale-_offset.x*scale);
    float x1 = (GetGlobalPosition().x+(viewport.size.x/2.0f)/scale+_offset.x*scale);
    float y0 = (GetGlobalPosition().y-(viewport.size.y/2.0f)/scale-_offset.y*scale);
    float y1 = (GetGlobalPosition().y+(viewport.size.y/2.0f)/scale+_offset.y*scale);
    return ufo::Rectangle({x0, y0}, {x1-x0, y1-y0});
}

ufo::gc::JsonMap* Camera::GetAsJson(ufo::GarbageCollector* _gc){

    ufo::gc::JsonMap* parent_class_as_json = Actor::GetAsJson(_gc);

    parent_class_as_json->map.emplace("clamp",_gc->New<ufo::gc::JsonNumber>(clamp));
    parent_class_as_json->map.emplace("scale",_gc->New<ufo::gc::JsonNumber>(scale));

    return parent_class_as_json;
}

#ifdef UFO_ENGINE_STUDIO

void Camera::OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){

    Actor::OnViewProperties(_level_editor_tab, _index);

    ImGui::Checkbox("Clamp", &clamp);
    ImGui::InputFloat("Scale", &scale);

}

#endif

}
