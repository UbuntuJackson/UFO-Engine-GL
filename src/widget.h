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

    void OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera){

    }

};

}
