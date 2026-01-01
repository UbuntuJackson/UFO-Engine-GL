#pragma once
#include <string>

struct TTF_Font;

namespace ufo{

class Font{
private:
    TTF_Font* font = nullptr;
public:
    Font(const std::string& _path, float _size);

    TTF_Font* GetFont();

    ~Font();

};

}
