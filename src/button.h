#pragma once
#include "text.h"
#include "../ufo_maths/ufo_maths.h"

namespace ufo{


class Button : public Text{
public:

    Button(Vector2f _);
    bool IsHovered();
    bool IsPressed();
    bool IsHeld();
    bool IsReleased();

};



}
