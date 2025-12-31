#pragma once
#include <vector>
#include <memory>
#include "../ufo_maths/ufo_maths.h"
#include "graphics.h"
#include "actor.h"

namespace ufo{

class Widget : public Actor{
public:
    Widget(Vector2f _) : Actor(_){}

};

}
