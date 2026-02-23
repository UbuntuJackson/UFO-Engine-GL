#include <vector>
#include <memory>
#include "../ufo_maths/ufo_maths.h"
#include "graphics.h"
#include "actor.h"
#include "camera.h"
#include "../ufo_engine_studio/level_editor_tab.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui.h"
#include "widget.h"
#include "engine.h"

namespace ufo{

Widget::Widget(Vector2f _) : Actor(_){
    class_name = "ufo::Widget";
    base_class_name = class_name;
}

ufo::Rectangle Widget::GetRectangle(){
    return ufo::Rectangle(GetGlobalPosition()+rectangle.position,rectangle.size);
}

void Widget::OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab){

}

void Widget::OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){

}

void Widget::ResizeOrMove(Vector2f _screen_space_mouse_position){
    const float margin = 10.0f; //Pixels.
    const Vector2f margin_vector = Vector2f(margin, margin);

    const ufo::Rectangle move_rectangle = ufo::Rectangle(GetGlobalPosition()+rectangle.position+margin_vector, rectangle.size-margin_vector*2.0f);

    const ufo::Rectangle resize_rectangle_left = ufo::Rectangle(GetGlobalPosition()+rectangle.position+Vector2f(0.0f, margin), Vector2f(margin, move_rectangle.size.y));

    const ufo::Rectangle resize_rectangle_right = ufo::Rectangle(Vector2f(move_rectangle.position.x+move_rectangle.size.x, move_rectangle.position.y), Vector2f(margin, move_rectangle.size.y));

    const ufo::Rectangle resize_rectangle_top = ufo::Rectangle(move_rectangle.position-Vector2f(0.0f, margin), Vector2f(move_rectangle.size.x, margin));

    const ufo::Rectangle resize_rectangle_bottom = ufo::Rectangle(move_rectangle.position+Vector2f(0.0f, move_rectangle.size.y), Vector2f(move_rectangle.size.x, margin));

    const ufo::Rectangle resize_rectangle_top_left = ufo::Rectangle(move_rectangle.position-margin_vector, margin_vector);
    const ufo::Rectangle resize_rectangle_top_right = ufo::Rectangle(resize_rectangle_top_left.position+Vector2f(resize_rectangle_top.size.x+margin, 0.0f), margin_vector);
    const ufo::Rectangle resize_rectangle_bottom_left = ufo::Rectangle(resize_rectangle_top_left.position+Vector2f(0.0f, resize_rectangle_left.size.y+margin), margin_vector);
    const ufo::Rectangle resize_rectangle_bottom_right = ufo::Rectangle(move_rectangle.position+move_rectangle.size, margin_vector);

    if(ufo::Maths::RectangleVsPoint(move_rectangle,_screen_space_mouse_position)){


        Vector2f dp = engine->mouse.position - engine->mouse.former_position;

        local_position += dp;

        Console::PrintLine("Move Rectangle");
    }
    if(ufo::Maths::RectangleVsPoint(resize_rectangle_left,_screen_space_mouse_position)){


        Vector2f dp = engine->mouse.position - engine->mouse.former_position;

        rectangle.size.x -= dp.x;
        rectangle.position.x += dp.x;


        Console::PrintLine("Resize left");
    }
    if(ufo::Maths::RectangleVsPoint(resize_rectangle_right,_screen_space_mouse_position)){

        Vector2f dp = engine->mouse.position - engine->mouse.former_position;

        rectangle.size.x += dp.x;

        Console::PrintLine("Resize right");
    }
    if(ufo::Maths::RectangleVsPoint(resize_rectangle_top,_screen_space_mouse_position)){
        Console::PrintLine("Resize top");
    }
    if(ufo::Maths::RectangleVsPoint(resize_rectangle_bottom,_screen_space_mouse_position)){
        Console::PrintLine("Resize bottom");

    }

    if(ufo::Maths::RectangleVsPoint(resize_rectangle_top_left,_screen_space_mouse_position)){

        Vector2f dp = engine->mouse.position - engine->mouse.former_position;

        rectangle.position += dp;
        rectangle.size -= dp;

        Console::PrintLine("Resize top left");
    }

    if(ufo::Maths::RectangleVsPoint(resize_rectangle_top_right,_screen_space_mouse_position)){
        Console::PrintLine("Resize top right");
    }

    if(ufo::Maths::RectangleVsPoint(resize_rectangle_bottom_left,_screen_space_mouse_position)){

        Console::PrintLine("Resize bottom left");
    }

    if(ufo::Maths::RectangleVsPoint(resize_rectangle_bottom_right,_screen_space_mouse_position)){

        Vector2f dp = engine->mouse.position - engine->mouse.former_position;

        rectangle.size += dp;

        Console::PrintLine("Resize bottom right");
    }

}

void Widget::OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){

    Actor::OnUpdateEditorViewport(_editor, _level_editor_tab);

    {
        Vector2f pos_min = GetGlobalPosition()+rectangle.position;
        Vector2f pos_max = GetGlobalPosition()+rectangle.position+rectangle.size;
        Vector2f global_position = GetGlobalPosition();

        //Console::PrintLine(engine->mouse.GetPosition());

        ImVec2 im_viewport_pos = ImGui::GetItemRectMin();

        //float width_through_height = 1920.0f/1080.0f

        Vector2f viewport_pos = Vector2f(im_viewport_pos.x, im_viewport_pos.y);

        ImU32 colour = 0xFFFFFFFF;
        if(parent->base_class_name != "ufo::Level") colour = 0xFF664422;

        ImGui::GetWindowDrawList()->AddRect(ImVec2(viewport_pos.x+pos_min.x, viewport_pos.y+pos_min.y),ImVec2(viewport_pos.x+pos_max.x, viewport_pos.y+pos_max.y), colour);

        ImGui::GetWindowDrawList()->AddLine(ImVec2(global_position.x+viewport_pos.x, global_position.y+viewport_pos.y-5.0f), ImVec2(global_position.x+viewport_pos.x, global_position.y+viewport_pos.y+5.0f), 0xFF0000FF, 1.0f);
        ImGui::GetWindowDrawList()->AddLine(ImVec2(global_position.x+viewport_pos.x-5.0f, global_position.y+viewport_pos.y), ImVec2(global_position.x+viewport_pos.x+5.0f, global_position.y+viewport_pos.y), 0xFF0000FF, 1.0f);
    }

    ImVec2 im_viewport_pos = ImGui::GetItemRectMin();

    Vector2f viewport_pos = Vector2f(im_viewport_pos.x, im_viewport_pos.y);

    ImVec2 window_pos = ImGui::GetMainViewport()->Pos;

    Vector2f editor_viewport_pos = Vector2f(viewport_pos.x-window_pos.x,viewport_pos.y-window_pos.y);

    Vector2f mouse_position_over_screenspace = engine->mouse.position-editor_viewport_pos;

    Vector2f world_mouse = mouse_position_over_screenspace;
    Vector2f former_world_mouse = engine->mouse.former_position-editor_viewport_pos;

    if(ufo::Maths::RectangleVsPoint(ufo::Rectangle(GetGlobalPosition()+rectangle.position, rectangle.size),world_mouse)){
        //Console::PrintLine("Overlapping");

        if(engine->mouse.is_left_button_held){
            ResizeOrMove(world_mouse);
            IrregularUpdate();
        }

        /*if(engine->mouse.is_left_button_held){

            Vector2f dp = world_mouse - former_world_mouse;

            local_position += dp;

            }*/

    }

}

ufo::gc::JsonMap* Widget::GetAsJson(ufo::GarbageCollector* _gc){
    Console::PrintLine("Does this even run?");

    ufo::gc::JsonMap* parent_class_as_json = Actor::GetAsJson(_gc);

    auto j_rectangle = _gc->New<ufo::gc::JsonMap>();
    j_rectangle->map.emplace("x", _gc->New<ufo::gc::JsonNumber>(rectangle.position.x));
    j_rectangle->map.emplace("y", _gc->New<ufo::gc::JsonNumber>(rectangle.position.y));
    j_rectangle->map.emplace("w", _gc->New<ufo::gc::JsonNumber>(rectangle.size.x));
    j_rectangle->map.emplace("h", _gc->New<ufo::gc::JsonNumber>(rectangle.size.y));

    parent_class_as_json->map.emplace("rectangle", j_rectangle);
    return parent_class_as_json;
}

}
