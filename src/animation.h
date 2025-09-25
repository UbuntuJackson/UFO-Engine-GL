#pragma once
#include <unordered_map>
#include "ufo_maths.h"
#include "graphics.h"
#include "actor.h"
#include "sprite.h"

class Animation : public Actor{
public:

    //Make a special type of Sprite with overwritten Draw function to not have the OnDraw callback call by default
    Sprite* current_animation = nullptr;

    std::unordered_map<std::string ,Sprite*> sprites;

    Animation(Vector2f _local_position) : Actor(_local_position){

    }

    void OnSpawn(){

        

    }

    void OnDraw(ufo::Graphics* _graphics, Camera* _camera){

        current_animation->OnDraw(_graphics, _camera);

    }
};