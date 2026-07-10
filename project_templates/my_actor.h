// Add extension .ufo.h to expose class to UFO-Engine Studio
/* Write author name and date here */

#pragma once
#include <ufo_macros.h>
#include <ufo_maths.h>
#include <console.h>
//#include <actor.h>
//#include <animation.h>
//#include <sprite.h>
//#include "../generated_base_classes.h"

//namespace MyGame{

// ufo_actor_config("my_actor.ason")
// ufo_category("My Actors")
// ufo_comment("Enter your description here")
// ufo_hide_in_editor()
ufo_class() class MyActor : public BaseClass{
public:

    // Mandatory constructor
    MyActor(Vector2f _);

    // Called when actor enters level
    void OnSpawn();

    // Called every time engine updates level
    void OnUpdate(float _delta_time) override;

};

//} //namespace MyGame
