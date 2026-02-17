/* Write author name and date here */

#pragma once
#include <ufo_macros.h>
#include <actor.h>
#include <ufo_maths.h>
#include <console.h>

// Add extension .ufo.h to expose class to UFO-Engine Studio

// ufo_actor_config("src/my_actor.ason") ufo_category("My Actors")
// ufo_comment("This is an editor description of this actor")
ufo_class() class MyActor : public Actor{
public:

    // Mandatory constructor
    MyActor(Vector2f _) : Actor(_){}

    // Called when actor enters level
    void OnSpawn() override{
        Actor::OnSpawn();

        Console::PrintLine("MyActor Spawned");
    }

    // Called every time engine updates level
    void OnUpdate(float _delta_time) override{
        Actor::OnUpdate(_delta_time);

        Console::PrintLine("MyActor Delta Time:",_delta_time);
    }

};
