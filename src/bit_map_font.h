#pragma once
#include "console.h"
#include "texture_2d.h"
#include "ufo_macros.h"
#include "ufo_maths.h"
#include <locale>
#include <map>
#include <sstream>
#include <string>
#include "../shapes/rectangle.h"
#include <bitset>
#include "graphics.h"

namespace ufo{

class OpenGLv4_5_AssetManager;
class Graphics;

class BitMapFont{
public:

    //Letter, Width
    std::map<char, int> width_for_characters;

    OpenGLv4_5_AssetManager* asset_manager = nullptr;
    int character_width = 0;
    int character_height = 0;
    std::string texture_key;

    BitMapFont(OpenGLv4_5_AssetManager* _asset_manager, const std::string& _texture_key, int _character_width, int _character_height);

    ufo::Rectangle
    GetRectangleFromPositionAndFrameSize(int _x, int _y, Vector2f _frame_size);

    ufo::Rectangle
    GetFrameFromSpriteSheet(std::string _sprite_key, int _frame, Vector2f _frame_size);

    void Draw(
        ufo::Graphics* _graphics,
        const std::string& _text,
        Vector2f _position,
        Vector2f _scale,
        const std::string& _shader_key,
        const ufo::Colour& _tint,
        bool _is_wrapping = false,
        int _wrap_width = 1000
    );

};

}
