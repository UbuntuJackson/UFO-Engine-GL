#include <memory>
#include "../glad/include/glad/glad.h"
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include "../utils/console.h"
#include "openglv4_5_asset_manager.h"
#include "sprite_renderer.h"
#include "engine.h"
#include "input.h"

namespace ufo{

Engine::Engine(int _width, int _height) :
    width{_width},
    height{_height}
{
    
}

void
Engine::Init(){
    text_renderer.Init(this);
    //Reserve space for a few dozens of actors or so
    level->actors.reserve(50);
    level->engine = this;
    Console::PrintLine("Space for number of actors:", level->actors.size());
}

Engine::~Engine(){
    Console::PrintLine("ufo::Application::~Application() ran");
}

struct BrushRectangle{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    bool is_on_canvas = true;
};

BrushRectangle CutOutRectangle(BrushRectangle _borders, BrushRectangle _brush){
    
    //check if all  points in rectangle
    
    int new_x = _brush.x;
    int new_y = _brush.y;
    int new_w = _brush.w;
    int new_h = _brush.h;
    
    if(_brush.x < 0){
        new_w = _brush.w + _brush.x;
        new_x = 0;
    }
    if(_brush.y < 0){
        new_h = _brush.h + _brush.y;
        new_y = 0;
    }
    if(_brush.x + _brush.w > _borders.w){
        new_w = _borders.w - _brush.x;
    }
    if(_brush.y + _brush.h > _borders.h){
        new_h = _borders.h - _brush.y;
    }

    if(new_w < 0 || new_h < 0){
        return BrushRectangle{new_x, new_y, new_w, new_h, false};
    }
    
    return BrushRectangle{new_x, new_y, new_w, new_h};
    
}

void Engine::Update(){
    if(keyboard.GetKey(SDLK_ESCAPE).is_pressed) quit = true;

    // Handle Timing
    m_tp2 = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsedTime = m_tp2 - m_tp1;
    m_tp1 = m_tp2;

    // Our time per frame coefficient
    float fElapsedTime = elapsedTime.count();
    fLastElapsed = fElapsedTime;

    level->UpdatePhrase(fLastElapsed);

    //Console::PrintLine(1.0f/fLastElapsed);
}

void Engine::UpdatePixelGameEngine(float _delta_time){

    level->UpdatePhrase(_delta_time);

}

void Engine::BrushTest(){
    if(mouse.is_left_button_held){

        auto& texture = asset_manager.textures.at("face");
    
        int brush_width = 64;
        int brush_height = 64;

        BrushRectangle br = CutOutRectangle(BrushRectangle{0,0,int(texture.width), int(texture.height)}, BrushRectangle{int(mouse.GetPosition().x), int(mouse.GetPosition().y), brush_width, brush_height});
        
        if(br.is_on_canvas){

            unsigned int new_data[br.h][br.w];

            for(int y = 0; y < br.h; y++){
                for(int x = 0; x < br.w; x++){
                    new_data[y][x] = 0xFF0000FF;
                }
            }
            
            glTexSubImage2D(GL_TEXTURE_2D, 0, br.x, br.y, br.w, br.h, GL_RGBA, GL_UNSIGNED_BYTE, (void*)new_data);
        }
        
    }

    unsigned int pixel_data[4*1] = {0,0,0,0};

    glReadPixels(0,0, 1,1, GL_RGBA, GL_UNSIGNED_BYTE, (void*)(&pixel_data));

    //for(int i = 0; i < 4*1; i++) Console::Print(GLuint(pixel_data[i]), " ");
    //Console::PrintLine("");

}

void Engine::Render(){

    level->DrawPhase(graphics.get());
    graphics->DrawPartialSprite("face", Vector2f(50.0f, 50.0f), Vector2f(800.0f, 600.0f), Vector2f(0.0f, 0.0f), Vector2f(0.5f, 0.5f),
        Vector2f(600.0f, 0.0f),Vector2f(200.0f, 600.0f) , 0.0f, ufo::Colour(255,255,255,255));

    text_renderer.Test();

}

}