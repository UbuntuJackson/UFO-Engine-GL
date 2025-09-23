#pragma once
#include "../ufo_maths/ufo_maths.h"
#include "glm/glm.hpp"
#include "actor.h"
#include "graphics.h"
#include "engine.h"
#include "camera.h"
#include "../random/random_number_generator.h"

class ColourCircle : public Actor{
public:
    float radius = 10.0f;
    ufo::Colour colour = ufo::Colour(100, 0, 100, 250);

    ColourCircle(Vector2f _local_position) : Actor(_local_position){
        
    }

    void OnUpdate(float _delta_time){
        //int i = RandomNumberGenerator::Get().RandomInt(0, 200);
        //if(i == 0) is_dead = true;
    }

    void OnDraw(ufo::Graphics* _graphics, Camera* _camera){
        _graphics->DrawCircle(_camera->Transform(GetGlobalPosition()), radius*_camera->scale, colour);
    }

};