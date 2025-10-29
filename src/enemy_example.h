#pragma once
#include "ufo_macros.h"
#include "actor.h"
#include "graphics.h"
#include "../ufo_maths/ufo_maths.h"

namespace game{

ufo_category("Enemies") class Knight : public Actor{
public:

    ufo_variable() float speed = 500.0f;

    ufo_alias("Type of Knight") ufo_radio_button("chase", "zombie", "screensaver")
    int type_of_knight = 0;

    ufo_alias("Name")
    std::string name = "";

    ufo_comment("This is just a test variable lol")
    int a = 0;

    Vector2f home_position = Vector2f(0.0f, 0.0f);

    /*Knight(Vector2f _local_position) : Actor(_local_position){}*/

    Level* level = nullptr;
    
    void OnSpawn(){
        Actor::OnSpawn();

        level = level;
    }

    void OnUpdate(float _delta_time){
        local_position += Vector2f(1.0f, 0.0f) * speed * _delta_time;
    }

};

ufo_class() class ColourRectangle : public Actor{
public:

    ufo_alias("colour") int colour_hex = 0xFF0000FF;

};

}