#pragma once
#include <memory>
#include <chrono>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "../src/engine.h"

//#define USE_PGE

#ifndef USE_PGE

namespace ufo{

class Main{
public:
    SDL_Window* window = nullptr;
    SDL_GLContext open_gl_context;
    std::unique_ptr<Engine> engine = nullptr;

Main();

void Start(std::unique_ptr<Engine> _custom_engine);

};
}
#endif

#ifdef USE_PGE
#include "pixel_game_engine/pixel_game_engine.h"
#include "pixel_game_engine/pge_graphics.h"

int main(){
    //Need different version of Engine class?

    ufo::Engine engine(800,600);
    ufo::PixelGameEngine pge(&engine);
    engine.graphics = std::make_unique<ufo::PixelGameEngineGraphics>(&engine, &pge);
    
    if(pge.Construct(800,600,1,1,false,true,true)){
        pge.Start();
    }
}

#endif