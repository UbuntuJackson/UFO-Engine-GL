#pragma once
#include "../ufo_maths/ufo_maths.h"
#include "actor.h"

class Sprite : public Actor{
public:
    std::string texture_key = "";

    Sprite(Vector2f _local_position) : Actor(_local_position){
        
    }

    void OnDraw(/*Camera*/){

    }
};