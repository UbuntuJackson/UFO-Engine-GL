#include "../shapes/rectangle.h"
#include "../ufo_maths/ufo_maths.h"
#include "actor.h"
#include "engine.h"
#include "input.h"
#include "viewport_editing_utils.h"
#include "../../imgui/imgui.h"

namespace ufo{

void ResizeOrMove(Actor* _actor,
    PartsOfRectangle& _part_of_rectangle_resized_in_editor,
    Vector2f _position_screen_space,
    Vector2f _size_screen_space,
    Vector2f& _rectangle_position,
    Vector2f& _rectangle_size,
    Vector2f _screen_space_mouse_position,
    Vector2f _scaled_delta_mouse_position
){
    const float margin = 10.0f; //Pixels.
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

    if(_actor->engine->mouse.is_left_button_released) _part_of_rectangle_resized_in_editor = PartsOfRectangle::NONE;

    //MIDDLE section not necessary
    if(ufo::Maths::RectangleVsPoint(move_rectangle,_screen_space_mouse_position)){
        return;
        //if(_actor->engine->mouse.is_left_button_pressed) _part_of_rectangle_resized_in_editor = PartsOfRectangle::MIDDLE;

    }
    if(ufo::Maths::RectangleVsPoint(resize_rectangle_left,_screen_space_mouse_position)){
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

        if(_actor->engine->mouse.is_left_button_pressed) _part_of_rectangle_resized_in_editor = PartsOfRectangle::LEFT;


        Console::PrintLine("Resize left");
    }
    if(ufo::Maths::RectangleVsPoint(resize_rectangle_right,_screen_space_mouse_position)){
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

        if(_actor->engine->mouse.is_left_button_pressed) _part_of_rectangle_resized_in_editor = PartsOfRectangle::RIGHT;

        Console::PrintLine("Resize right");
    }
    if(ufo::Maths::RectangleVsPoint(resize_rectangle_top,_screen_space_mouse_position)){
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);

        if(_actor->engine->mouse.is_left_button_pressed) _part_of_rectangle_resized_in_editor = PartsOfRectangle::TOP;
        Console::PrintLine("Resize top");
    }
    if(ufo::Maths::RectangleVsPoint(resize_rectangle_bottom,_screen_space_mouse_position)){
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);

        if(_actor->engine->mouse.is_left_button_pressed) _part_of_rectangle_resized_in_editor = PartsOfRectangle::BOTTOM;
        Console::PrintLine("Resize bottom");

    }

    if(ufo::Maths::RectangleVsPoint(resize_rectangle_top_left,_screen_space_mouse_position)){
        if(_actor->engine->mouse.is_left_button_pressed) _part_of_rectangle_resized_in_editor = PartsOfRectangle::TOP_LEFT;
        Console::PrintLine("Resize top left");
    }

    if(ufo::Maths::RectangleVsPoint(resize_rectangle_top_right,_screen_space_mouse_position)){
        if(_actor->engine->mouse.is_left_button_pressed) _part_of_rectangle_resized_in_editor = PartsOfRectangle::TOP_RIGHT;
        Console::PrintLine("Resize top right");
    }

    if(ufo::Maths::RectangleVsPoint(resize_rectangle_bottom_left,_screen_space_mouse_position)){
        if(_actor->engine->mouse.is_left_button_pressed) _part_of_rectangle_resized_in_editor = PartsOfRectangle::BOTTOM_LEFT;
        Console::PrintLine("Resize bottom left");
    }

    if(ufo::Maths::RectangleVsPoint(resize_rectangle_bottom_right,_screen_space_mouse_position)){
        if(_actor->engine->mouse.is_left_button_pressed) _part_of_rectangle_resized_in_editor = PartsOfRectangle::BOTTOM_RIGHT;
        Console::PrintLine("Resize bottom right");
    }

    Vector2f dp = _scaled_delta_mouse_position;

    switch(_part_of_rectangle_resized_in_editor){
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

            _actor->local_position += dp;
            break;
        }
    }

}

}
