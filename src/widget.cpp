#include <vector>
#include <memory>
#include "../ufo_maths/ufo_maths.h"
#include "graphics.h"
#include "actor.h"
#include "camera.h"
#include "widget.h"
#include "engine.h"

#ifdef UFO_ENGINE_STUDIO
#include "../ufo_engine_studio/level_editor_tab.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui.h"
#endif

namespace ufo{

Widget::Widget(Vector2f _) : Actor(_){
    class_name = "ufo::Widget";
    base_class_name = class_name;
}

ufo::Rectangle Widget::GetRectangle(){
    return ufo::Rectangle(GetGlobalPosition()+rectangle.position,rectangle.size);
}

void Widget::OnLoadDefaultProperties(ufo::gc::JsonMap* _json){
    //A good example of large amount of properties being written to an object
    // Potential solution, have an additional map which handles writing of default properties.
    // Other solution, pass json. I like this solution more, because that makes the generated code more managable.
    // Son of a biscuit this has been redundant.
    // Writing of custom properties handled in generated.h.

    try{
        auto j_rectangle = _json->map.at("rectangle")->AsMap();
        float widget_x = j_rectangle.at("x")->AsFloat();
        float widget_y = j_rectangle.at("y")->AsFloat();
        float widget_w = j_rectangle.at("w")->AsFloat();
        float widget_h = j_rectangle.at("h")->AsFloat();
        rectangle.position.x = widget_x;
        rectangle.position.y = widget_y;
        rectangle.size.x = widget_w;
        rectangle.size.y = widget_h;
    } catch(const std::exception& _error){
        Console::PrintLine("[UFO-Engine] GenericGenerator: Could not find properties for json representing ufo::Widget instance");
    }
}

ufo::gc::JsonMap* Widget::GetAsJson(ufo::GarbageCollector* _gc){

    ufo::gc::JsonMap* parent_class_as_json = Actor::GetAsJson(_gc);

    auto j_rectangle = _gc->New<ufo::gc::JsonMap>();
    j_rectangle->map.emplace("x", _gc->New<ufo::gc::JsonNumber>(rectangle.position.x));
    j_rectangle->map.emplace("y", _gc->New<ufo::gc::JsonNumber>(rectangle.position.y));
    j_rectangle->map.emplace("w", _gc->New<ufo::gc::JsonNumber>(rectangle.size.x));
    j_rectangle->map.emplace("h", _gc->New<ufo::gc::JsonNumber>(rectangle.size.y));

    parent_class_as_json->map.emplace("rectangle", j_rectangle);
    return parent_class_as_json;
}

#ifdef UFO_ENGINE_STUDIO

void Widget::OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab){

}

void Widget::OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){

}

void Widget::ResizeOrMove(Vector2f _position_screen_space,Vector2f _size_screen_space, Vector2f& _rectangle_position, Vector2f& _rectangle_size, Vector2f _screen_space_mouse_position){
    const float margin = 20.0f; //Pixels.
    const Vector2f margin_vector = Vector2f(margin, margin);

    const ufo::Rectangle move_rectangle = ufo::Rectangle(_position_screen_space+margin_vector, _size_screen_space-margin_vector*2.0f);

    const ufo::Rectangle resize_rectangle_left = ufo::Rectangle(_position_screen_space+Vector2f(0.0f, margin), Vector2f(margin, move_rectangle.size.y));

    const ufo::Rectangle resize_rectangle_right = ufo::Rectangle(Vector2f(move_rectangle.position.x+move_rectangle.size.x, move_rectangle.position.y), Vector2f(margin, move_rectangle.size.y));

    const ufo::Rectangle resize_rectangle_top = ufo::Rectangle(move_rectangle.position-Vector2f(0.0f, margin), Vector2f(move_rectangle.size.x, margin));

    const ufo::Rectangle resize_rectangle_bottom = ufo::Rectangle(move_rectangle.position+Vector2f(0.0f, move_rectangle.size.y), Vector2f(move_rectangle.size.x, margin));

    const ufo::Rectangle resize_rectangle_top_left = ufo::Rectangle(move_rectangle.position-margin_vector, margin_vector);
    const ufo::Rectangle resize_rectangle_top_right = ufo::Rectangle(resize_rectangle_top_left.position+Vector2f(resize_rectangle_top.size.x+margin, 0.0f), margin_vector);
    const ufo::Rectangle resize_rectangle_bottom_left = ufo::Rectangle(resize_rectangle_top_left.position+Vector2f(0.0f, resize_rectangle_left.size.y+margin), margin_vector);
    const ufo::Rectangle resize_rectangle_bottom_right = ufo::Rectangle(move_rectangle.position+move_rectangle.size, margin_vector);

    if(engine->mouse.is_left_button_released) part_of_rectangle_resized_in_editor = PartsOfRectangle::NONE;

    if(ufo::Maths::RectangleVsPoint(move_rectangle,_screen_space_mouse_position)){

        if(engine->mouse.is_left_button_pressed) part_of_rectangle_resized_in_editor = PartsOfRectangle::MIDDLE;

    }
    if(ufo::Maths::RectangleVsPoint(resize_rectangle_left,_screen_space_mouse_position)){


        if(engine->mouse.is_left_button_pressed) part_of_rectangle_resized_in_editor = PartsOfRectangle::LEFT;


        Console::PrintLine("Resize left");
    }
    if(ufo::Maths::RectangleVsPoint(resize_rectangle_right,_screen_space_mouse_position)){

        if(engine->mouse.is_left_button_pressed) part_of_rectangle_resized_in_editor = PartsOfRectangle::RIGHT;

        Console::PrintLine("Resize right");
    }
    if(ufo::Maths::RectangleVsPoint(resize_rectangle_top,_screen_space_mouse_position)){
        if(engine->mouse.is_left_button_pressed) part_of_rectangle_resized_in_editor = PartsOfRectangle::TOP;
        Console::PrintLine("Resize top");
    }
    if(ufo::Maths::RectangleVsPoint(resize_rectangle_bottom,_screen_space_mouse_position)){
        if(engine->mouse.is_left_button_pressed) part_of_rectangle_resized_in_editor = PartsOfRectangle::BOTTOM;
        Console::PrintLine("Resize bottom");

    }

    if(ufo::Maths::RectangleVsPoint(resize_rectangle_top_left,_screen_space_mouse_position)){
        if(engine->mouse.is_left_button_pressed) part_of_rectangle_resized_in_editor = PartsOfRectangle::TOP_LEFT;
        Console::PrintLine("Resize top left");
    }

    if(ufo::Maths::RectangleVsPoint(resize_rectangle_top_right,_screen_space_mouse_position)){
        if(engine->mouse.is_left_button_pressed) part_of_rectangle_resized_in_editor = PartsOfRectangle::TOP_RIGHT;
        Console::PrintLine("Resize top right");
    }

    if(ufo::Maths::RectangleVsPoint(resize_rectangle_bottom_left,_screen_space_mouse_position)){
        if(engine->mouse.is_left_button_pressed) part_of_rectangle_resized_in_editor = PartsOfRectangle::BOTTOM_LEFT;
        Console::PrintLine("Resize bottom left");
    }

    if(ufo::Maths::RectangleVsPoint(resize_rectangle_bottom_right,_screen_space_mouse_position)){
        if(engine->mouse.is_left_button_pressed) part_of_rectangle_resized_in_editor = PartsOfRectangle::BOTTOM_RIGHT;
        Console::PrintLine("Resize bottom right");
    }

    Vector2f dp = engine->mouse.position - engine->mouse.former_position;

    switch(part_of_rectangle_resized_in_editor){
        case PartsOfRectangle::TOP:{
            _rectangle_position.y+=dp.y;
            _rectangle_size.y-=dp.y;
            break;
        }
        case PartsOfRectangle::BOTTOM:{
            _rectangle_size.y+=dp.y;
            break;
        }
        case PartsOfRectangle::RIGHT:{
            _rectangle_size.x += dp.x;
            break;
        }
        case PartsOfRectangle::LEFT:{
            _rectangle_size.x -= dp.x;
            _rectangle_position.x += dp.x;
            break;
        }
        case PartsOfRectangle::TOP_LEFT:{

            _rectangle_position += dp;
            _rectangle_size += dp;
            break;
        }
        case PartsOfRectangle::TOP_RIGHT:{

            break;
        }
        case PartsOfRectangle::BOTTOM_LEFT:{
            break;
        }
        case PartsOfRectangle::BOTTOM_RIGHT:{
            _rectangle_size += dp;
            break;
        }
        case PartsOfRectangle::MIDDLE:{

            local_position += dp;
            break;
        }
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

    ResizeOrMove(GetGlobalPosition()+rectangle.position, rectangle.size, rectangle.position, rectangle.size, world_mouse);

    if(ufo::Maths::RectangleVsPoint(ufo::Rectangle(GetGlobalPosition()+rectangle.position, rectangle.size),world_mouse)){
        //Console::PrintLine("Overlapping");

        if(engine->mouse.is_left_button_held){

            IrregularUpdate();
        }

    }

}
#endif //UFO_ENGINE_STUDIO

}
