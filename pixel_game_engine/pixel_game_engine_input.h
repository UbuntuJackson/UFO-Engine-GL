#pragma once
#include "../src/input.h"

class PixelGameEngineMouse : public Mouse{
public:
    PixelGameEngineMouse(){}
    PixelGameEngineMouse(PixelGameEngineMouse&&) = delete;
    PixelGameEngineMouse(const PixelGameEngineMouse&) = delete;

    void* pixel_game_engine;

    void CheckEvents(SDL_Event& event){
        
    }

    void ClearPressedAndReleased(){
        
    }

    glm::vec2 GetPosition(){
        float x = 0;
        float y = 0;

        SDL_GetMouseState(&x,&y);

        return glm::vec2(x,y);
    }
};

class PixelGameEngineKeyboard : public Keyboard{
public:
    PixelGameEngineKeyboard(){}
    PixelGameEngineKeyboard(PixelGameEngineKeyboard&&) = delete;
    PixelGameEngineKeyboard(const PixelGameEngineKeyboard&) = delete;

    void* pixel_game_engine;

    Keyboard::Key& GetKey(Uint8 _key_id){
        
    }

    void CheckEvents(SDL_Event& event){
        
    }

    void ClearPressedAndReleased(){
        
    }
};