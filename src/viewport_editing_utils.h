#pragma once
#include "../shapes/rectangle.h"
#include "../ufo_maths/ufo_maths.h"
#include "actor.h"

namespace ufo{

enum PartsOfRectangle{
    TOP,BOTTOM,RIGHT,LEFT,TOP_LEFT,TOP_RIGHT,BOTTOM_LEFT,BOTTOM_RIGHT,MIDDLE,NONE
};

void ResizeOrMove(
    Actor* _actor,
    PartsOfRectangle& _part_of_rectangle_resized_in_editor,
    Vector2f _position_screen_space,
    Vector2f _size_screen_space,
    Vector2f& _rectangle_position,
    Vector2f& _rectangle_size,
    Vector2f _screen_space_mouse_position,
    Vector2f _scaled_delta_mouse_position
);

}
