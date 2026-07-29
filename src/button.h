#pragma once
#include "text.h"
#include "../ufo_maths/ufo_maths.h"

namespace ufo{


class Button : public Text{
public:
    std::string next_widget_path;

    ufo::Actor* GetNextWidget(){
        return GetActor(next_widget_path);
    }

    Button(Vector2f _);
    bool IsHovered();
    bool IsPressed();
    bool IsHeld();
    bool IsReleased();

};



}
