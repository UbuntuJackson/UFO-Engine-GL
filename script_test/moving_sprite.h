#pragma once
#include "../src/script.h"
#include "../src/ufo_macros.h"
#include "../src/sprite.h"
#include "../utils/console.h"

ufo_script() class MovingSprite : public ufo::TemplateScript<ufo::Sprite>{
public:

    void OnSpawn() override{
        Console::PrintLine("MovingSprite::OnSpawn",GetHostActor());
    }

    void OnUpdate(float _delta_time) override{
        GetHostActor()->rotation += 100.0f * _delta_time;
    }
};
