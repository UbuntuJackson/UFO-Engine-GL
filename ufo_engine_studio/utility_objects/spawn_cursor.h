#pragma once
#include "../src/actor.h"
#include "../src/input.h"
#include "../src/engine.h"

namespace UFOEngineStudio{

class SpawnCursor : public Actor{
public:

    SpawnCursor() : Actor(Vector2f(0.0f, 0.0f)){}

    void OnUpdate(float _dt){
        local_position = engine->mouse.position;
    }

};

}
