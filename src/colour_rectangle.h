#pragma once
#include "../ufo_maths/ufo_maths.h"
#include "glm/glm.hpp"
#include "actor.h"
#include "graphics.h"
#include "../utils/console.h"
#include "input.h"
#include "engine.h"
#include "camera.h"

class ColourRectangle : public Actor{
public:
    Vector2f size = Vector2f(30.0f, 20.0f);
    ufo::Colour colour = ufo::Colour(255, 255, 255, 100);
    float rotation = 0.0f;
    Vector2f centre = Vector2f(0.0f, 0.0f);
    Vector2f scale = Vector2f(1.0f, 1.0f);

    ColourRectangle(Vector2f _local_position) : Actor(_local_position){
        
    }

    void OnSpawn(){
        Console::PrintLine("Level", level);
        Console::PrintLine("Engine", engine);
    }

    void OnUpdate(float _delta_time){
        
    }

    void OnDraw(ufo::Graphics* _graphics, Camera* _camera){
        _graphics->DrawRectangleExtra(_camera->Transform(GetGlobalPosition()), size*_camera->scale, centre, scale, rotation, colour);
    }
};