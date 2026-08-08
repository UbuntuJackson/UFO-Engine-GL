#include "console.h"
#include "graphics.h"
#include "openglv4_5_asset_manager.h"
#include "graphics.h"
#include "texture_2d.h"
#include "ufo_macros.h"
#include "../ufo_maths/ufo_maths.h"
#include <locale>
#include <map>
#include <sstream>
#include <string>
#include "graphics.h"
#include "../shapes/rectangle.h"
#include <bitset>
#include "bit_map_font.h"

namespace ufo{

BitMapFont::BitMapFont(OpenGLv4_5_AssetManager* _asset_manager, const std::string& _texture_key, int _character_width, int _character_height) :
asset_manager{_asset_manager},
texture_key{_texture_key},
character_width{_character_width},
character_height{_character_height}
{

    /*ufo::Texture2D& texture = _asset_manager->textures.at(_texture_key);

    unsigned char data[texture.width*texture.height*4] = {0};

    texture.Bind();

    glGetTexImage(
        GL_TEXTURE_2D,
        0,
        texture.image_format,
        GL_UNSIGNED_BYTE, data);

    int p = 0;

    ufo::Colour pixels[texture.width*texture.height];

    while(p < (int)texture.width*texture.height*4){
        Console::PrintLine("Pixel:", (int)data[p], (int)data[p+1],(int)data[p+2],(int)data[p+3]); //need some conditionals here
        pixels[p/4] = ufo::Colour((int)data[p], (int)data[p+1],(int)data[p+2],(int)data[p+3]);
        p+=4;
        }*/

}

ufo::Rectangle
BitMapFont::GetRectangleFromPositionAndFrameSize(int _x, int _y, Vector2f _frame_size){
    ufo::Rectangle rect = ufo::Rectangle({(float)(_x * _frame_size.x), (float)(_y * _frame_size.y)}, _frame_size);
    return rect;
}

ufo::Rectangle
BitMapFont::GetFrameFromSpriteSheet(std::string _sprite_key, int _frame, Vector2f _frame_size){
    int fx = 1;
    int fy = 1;
    //To be fixed, clamp the values for height and width
    if(_frame_size.x > 0.0f && _frame_size.y > 0.0f &&
        _frame_size.x <= asset_manager->textures.at(_sprite_key).width && _frame_size.y <= asset_manager->textures.at(_sprite_key).height){
        fx = (int)_frame % (asset_manager->textures.at(_sprite_key).width/(int)_frame_size.x); //1 can only give me x = 0
        fy = (int)_frame / (asset_manager->textures.at(_sprite_key).width/(int)_frame_size.x);
    }
    return GetRectangleFromPositionAndFrameSize(
        fx,
        fy,
        _frame_size); //1 can only give y = 1
}

void BitMapFont::Draw(ufo::Graphics* _graphics, const std::string& _text, Vector2f _position, Vector2f _scale, const std::string& _shader_key, const ufo::Colour& _tint, const ufo::Rectangle& _rectangle, bool _is_wrapping, int _wrap_width){

    int current_character_x = 0;
    int current_character_y = _rectangle.position.y;

    std::string::const_iterator it = _text.begin();

    while(it != _text.end()){
        if(_is_wrapping){
            if(current_character_x >= _wrap_width){
                current_character_y += character_height;
                current_character_x = 0;
            }
        }

        int prefix = 0;
        int number_of_bytes = 1;

        unsigned char character = *it;

        if(character >> 7 == 0){
            prefix = 0;
            number_of_bytes = 1;
        }
        if(character >> 5 == 6){
            prefix = 6;
            number_of_bytes = 2;
        }
        if(character >> 4 == 14){
            prefix = 14;
            number_of_bytes = 3;
        }
        if(character >> 3 == 30){
            prefix = 30;
            number_of_bytes = 4;
        }

        std::string s;
        for(int i = 0; i < number_of_bytes; i++){

            std::string this_byte_as_string = std::bitset<8>((unsigned char)(*(it+i))).to_string();

            s+=this_byte_as_string;
        }

        std::string s_code_point = s;

        if(number_of_bytes == 1){
            if(character == '\n'){
                current_character_x = 0;
                current_character_y += character_height;
                it+=1;
                continue;
            }
        }

        if(number_of_bytes == 2){
            s_code_point = "";
            s_code_point += s.substr(4, 4);
            s_code_point += s.substr(8+2, 6);
        }

        if(number_of_bytes == 3){
            s_code_point = "";
            s_code_point += s.substr(4, 4);
            s_code_point += s.substr(8+2, 6);
            s_code_point += s.substr(8+8+2, 6);
        }

        if(number_of_bytes == 4){
            s_code_point = "";
            s_code_point += s.substr(4, 4);
            s_code_point += s.substr(8+2, 6);
            s_code_point += s.substr(8+8+2, 6);
            s_code_point += s.substr(8+8+8+2, 6);
        }

        int code_point = std::stoi(s_code_point, nullptr, 2);

        ufo::Rectangle sample_rectangle = GetFrameFromSpriteSheet(texture_key,code_point,Vector2f(character_width, character_height));

        _graphics->DrawPartialSprite(
            texture_key,
            Vector2f(_position.x + current_character_x*_scale.x, _position.y + current_character_y*_scale.y),
            Vector2f(0.0f, 0.0f),
            _scale,
            sample_rectangle.position,
            sample_rectangle.size,
            0.0f,
            _tint,
            _shader_key, 0.0f
        );

        current_character_x += character_width;
        it+=number_of_bytes;
    }
}

}
