#pragma once
#include "../external/olcPixelGameEngine.h"
#include "../src/engine.h"

namespace ufo{
class PixelGameEngine : public olc::PixelGameEngine{
public:

    Engine* engine = nullptr;

    PixelGameEngine(Engine* _engine)
    {
        engine = _engine;
        sAppName = "UFO Engine";
    }

    bool OnUserCreate(){
        engine->level->Load();
        return true;
    }

    bool OnUserUpdate(float _delta_time){
        SetPixelMode(olc::Pixel::NORMAL);
        Clear(olc::BLACK);
        engine->UpdatePixelGameEngine(_delta_time);
        engine->Render();
        
        return true;
    }

};

}