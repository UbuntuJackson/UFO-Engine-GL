#pragma once
#include "Main.h"
#include "graphics.h"
#include "texture_2d.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL.h>

namespace ufo{

class Font{
private:
    TTF_Font* font = nullptr;
public:
    Font(const std::string& _path, float _size){
        font = TTF_OpenFont(_path.c_str(),_size);
    }

    TTF_Font* GetFont(){
        return font;
    }

    ~Font(){
        TTF_CloseFont(font);
    }

    ufo::Texture2D MakeTexture(const std::string& _path, const ufo::Colour& _colour ){

    }

};

}
