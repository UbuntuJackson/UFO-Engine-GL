#pragma once
#include "console.h"
#include "texture_2d.h"
#include "ufo_macros.h"
#include "../ufo_maths/ufo_maths.h"
#include <locale>
#include <map>
#include <sstream>
#include <string>
#include "../shapes/rectangle.h"
#include <bitset>
#include "graphics.h"
#include "frame_buffer_texture.h"

namespace ufo{

class OpenGLv4_5_AssetManager;
class Graphics;
class Engine;
class Widget;

class BitMapFont{
public:
    struct Character{
        int code_point;
        int x;
        int y;
    };

    struct UTF8Text{
        Vector2f area;
        std::vector<Character> characters;
    };

    FrameBufferTexture frame_buffer_texture;

    //Letter, Width
    std::map<char, int> width_for_characters;

    ufo::Texture2D cached_texture;

    OpenGLv4_5_AssetManager* asset_manager = nullptr;
    int character_width = 0;
    int character_height = 0;
    std::string texture_key;

    bool is_savable = false;
    bool is_global_asset = true;

    BitMapFont(OpenGLv4_5_AssetManager* _asset_manager, const std::string& _texture_key, int _character_width, int _character_height);

    ufo::Rectangle
    GetRectangleFromPositionAndFrameSize(int _x, int _y, Vector2f _frame_size);

    ufo::Rectangle
    GetFrameFromSpriteSheet(std::string _sprite_key, int _frame, Vector2f _frame_size);

    UTF8Text Draw(
        ufo::Engine* _engine,
        bool _refresh,
        ufo::Widget* _parent,
        ufo::Graphics* _graphics,
        const std::string& _text,
        Vector2f _position,
        Vector2f _scale,
        const std::string& _shader_key,
        const ufo::Colour& _tint,
        const ufo::Rectangle& _rectangle,
        bool _is_wrapping = false,
        int _wrap_width = 1000
    );

};

}
