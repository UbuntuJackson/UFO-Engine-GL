#include <string>
#include <SDL3_ttf/SDL_ttf.h>
#include "font.h"

namespace ufo{

Font::Font(const std::string& _path, float _size){
    font = TTF_OpenFont(_path.c_str(),_size);
}

TTF_Font* Font::GetFont(){
    return font;
}

Font::~Font(){
    TTF_CloseFont(font);
}

}
