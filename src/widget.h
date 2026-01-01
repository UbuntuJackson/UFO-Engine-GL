#pragma once
#include <vector>
#include <memory>
#include "../ufo_maths/ufo_maths.h"
#include "graphics.h"
#include "actor.h"
#include "camera.h"

namespace ufo{

class Widget : public Actor{
public:
    Widget(Vector2f _) : Actor(_){}

    ufo::Rectangle rectangle = ufo::Rectangle(Vector2f(0.0f, 0.0f), Vector2f(600.0f, 200.0f));

    void OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera){

    }

};

}
