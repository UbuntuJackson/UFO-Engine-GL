#pragma once
#include "../src/actor.h"
#include "dynamic_array.h"
#include "garbage_collector.h"
#include "object.h"
#include "ufo_maths.h"

namespace ufo::gc{

class RootActor : public Actor{
public:
    DynamicArray<Actor*> actors;

    RootActor(Vector2f _) : Actor(_){}

};

};
