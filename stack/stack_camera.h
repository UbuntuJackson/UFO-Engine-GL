#pragma once

#include "../ufo_maths/ufo_maths.h"

namespace ufo::stack{

class Camera{
public:
    Vector2f position;

    Camera(Vector2f _position) : position{_position}{}

};


}
