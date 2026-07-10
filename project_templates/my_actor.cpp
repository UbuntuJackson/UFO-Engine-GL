#include <ufo_macros.h>
#include <ufo_maths.h>
#include <console.h>
//#include <actor.h>
//#include <animation.h>
//#include <sprite.h>
//#include "../generated_base_classes.h"
#include "my_actor.h"

//namespace MyGame{

MyActor::MyActor(Vector2f _) : BaseClass(_){}

// Called when actor enters level
void MyActor::OnSpawn() {
    BaseClass::OnSpawn();

    Console::PrintLine("MyActor Spawned");
}

// Called every time engine updates level
void MyActor::OnUpdate(float _delta_time) override{
    BaseClass::OnUpdate(_delta_time);

    Console::PrintLine("MyActor Delta Time:",_delta_time);
}

//} //namespace MyGame
