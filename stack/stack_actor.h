#pragma once
#include "../ufo_maths/ufo_maths.h"
#include "../src/graphics.h"
#include "../src/camera.h"
#include <cstdint>

namespace ufo::stack{

class Actor{
public:
    Vector2f position;
    bool alive = true;

    std::uint8_t sprite = 0;
    std::uint8_t controls = 0;
    std::uint8_t movement = 0; //Basically object that can move and has gravity
    std::uint8_t camera = 0;

    Actor(Vector2f _position) : position{_position}{}

    void Update(float _delta_time){

    }

    void Draw(ufo::Graphics* _graphics, Camera* _camera){

    }

};

}
