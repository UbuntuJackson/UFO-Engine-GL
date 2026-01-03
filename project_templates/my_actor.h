/* Write author name and date here */

#pragma once
#include <ufo_macros.h>
#include <actor.h>
#include <ufo_maths.h>
#include <console.h>

// Add extension .ufo.h to expose class to UFO-Engine Studio

/*ufo_actor_config("src/my_actor.ason")*/ /*ufo_category("My Actors")*/
ufo_class() class MyActor : public Actor{
public:

    MyActor(Vector2f _) : Actor(_){}

    void OnSpawn(){
        Actor::OnSpawn();

        Console::PrintLine("MyActor Spawned");
    }

    void OnUpdate(float _delta_time){
        Actor::OnUpdate();

        Console::PrintLine("MyActor Delta Time:",_delta_time);
    }

};
