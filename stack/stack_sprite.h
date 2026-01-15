#pragma once
#include "../ufo_maths/ufo_maths.h"
namespace ufo::stack{

class Sprite{
public:
    bool alive = true;
    Vector2f position;

     Sprite(Vector2f _position) : position{_position}{}

};


}
