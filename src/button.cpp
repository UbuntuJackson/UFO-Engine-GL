#include "button.h"
#include "engine.h"
#include "text.h"
#include "input.h"
#include "../ufo_maths/ufo_maths.h"

namespace ufo{

Button::Button(Vector2f _) : Text(_){
    class_name = "Button";
    base_class_name = class_name;
}

bool Button::IsHovered(){

    return ufoMaths::RectangleVsPoint(rectangle, engine->mouse.position);

}

bool Button::IsPressed(){
    return IsHovered() && engine->mouse.is_left_button_pressed;
}

bool Button::IsHeld(){
    return IsHovered() && engine->mouse.is_left_button_held;
}

bool Button::IsReleased(){
    return IsHovered() && engine->mouse.is_left_button_released;
}

}
