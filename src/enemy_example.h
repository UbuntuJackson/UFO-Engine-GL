#pragma once
#include "ufo_macros.h"
#include "actor.h"
#include "graphics.h"
#include "../ufo_maths/ufo_maths.h"

namespace uj{

namespace game{

ufo_category("Enemies") class Knight : public ufo::Sprite, public ufo::gc::Object{
public:

    ufo_variable() float speed = -0.5f;

    ufo_variable() int iq = -50;

    ufo_alias("slider_alias") ufo_int_slider(0,-23) int slider = 4;

    enum TypeOfKnight{
        CHASE,
        ZOMBIE,
        SCREENSAVER
    };

    ufo_alias("Type of Knight") ufo_radio_button("chase", "zombie", "screensaver")
    int type_of_knight = 0;

    ufo_alias("Name")
    std::string name="Joel";

    ufo_comment("This is just a test variable lol")
    int a;

    ufo_variable() int b = -2;

    ufo_variable() Vector2f home_position = Vector2f(-0.0f, -0.0f);

    Vector2f outside_level = {100.0f, 50.0f};

    ufo_variable() ufo::Colour colour = ufo::Colour(5,5,5,5);

    ufo_variable() ufo::Colour favourite_colour;

    //Example of single-line comment

    /*Knight(Vector2f _local_position) : Actor(_local_position){}*/

    Knight(Vector2f _local_position) : Actor(_local_position){}

    Level* level = nullptr;

    void OnSpawn(){
        Actor::OnSpawn();

        level = level;
    }

    void OnUpdate(float _delta_time){
        local_position += Vector2f(1.0f, 0.0f) * speed * _delta_time;
    }

};

#include "..."

ufo_class() class ColourRectangle : public Actor{
public:

    ufo_alias("colour") int colour_hex = 0xFF0000FF;

};

}

}
