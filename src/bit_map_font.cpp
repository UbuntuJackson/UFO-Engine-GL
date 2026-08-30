#include "console.h"
#include "engine.h"
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
#include "frame_buffer_texture.h"
#include "engine.h"
#include "widget.h"

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

void BitMapFont::Draw(ufo::Engine* _engine, bool _refresh, ufo::Widget* _parent, ufo::Graphics* _graphics, const std::string& _text, Vector2f _position, Vector2f _scale, const std::string& _shader_key, const ufo::Colour& _tint, const ufo::Rectangle& _rectangle, bool _is_wrapping, int _wrap_width){
    struct Character{
        int code_point;
        int x;
        int y;
    };

    std::vector<Character> characters;

    int number_of_characters_in_longest_row = 0;

    if(_refresh){
        int current_character_x = 0;

        //position.y because sometimes windows have headers
        int current_character_y = _rectangle.position.y;

        std::string::const_iterator it = _text.begin();

        while(it != _text.end()){
            if(_is_wrapping){
                if(current_character_x >= _wrap_width){
                    if(current_character_x > number_of_characters_in_longest_row) number_of_characters_in_longest_row = current_character_x;

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

            //I think 1 byte is most likely ascii
            if(number_of_bytes == 1){
                if(character == '\n'){
                    if(current_character_x > number_of_characters_in_longest_row) number_of_characters_in_longest_row = current_character_x;
                    current_character_x = 0;
                    current_character_y += character_height;
                    it+=1;
                    continue;
                }
            }

            //Letters like å ä ö tends to take up two bytes
            if(number_of_bytes == 2){
                s_code_point = "";
                s_code_point += s.substr(4, 4);
                s_code_point += s.substr(8+2, 6);
            }

            //A lot of chinese and japanese characters have this byte length
            if(number_of_bytes == 3){
                s_code_point = "";
                s_code_point += s.substr(4, 4);
                s_code_point += s.substr(8+2, 6);
                s_code_point += s.substr(8+8+2, 6);
            }

            // I'm unsure what takes up 4 bytes.
            if(number_of_bytes == 4){
                s_code_point = "";
                s_code_point += s.substr(4, 4);
                s_code_point += s.substr(8+2, 6);
                s_code_point += s.substr(8+8+2, 6);
                s_code_point += s.substr(8+8+8+2, 6);
            }

            int code_point = std::stoi(s_code_point, nullptr, 2);

            current_character_x += character_width;
            characters.push_back(Character{code_point, current_character_x, current_character_y});

            it+=number_of_bytes;

        }

        bool failed_to_create_frame_buffer = false;

        if(frame_buffer_texture.is_initialised) glDeleteTextures(1,&frame_buffer_texture.id);

        unsigned int frame_buffer_object = frame_buffer_texture.CreateFrameBuffer(number_of_characters_in_longest_row, current_character_y, failed_to_create_frame_buffer);
        frame_buffer_texture.BindFrameBuffer(frame_buffer_object);

        glViewport(0,0,frame_buffer_texture.width, frame_buffer_texture.height);
        _graphics->SetProjection(0.0f, frame_buffer_texture.width, 0.0f, frame_buffer_texture.height);

        for(Character& character : characters){
            ufo::Rectangle sample_rectangle = GetFrameFromSpriteSheet(texture_key,character.code_point,Vector2f(character_width, character_height));

            _graphics->DrawPartialSprite(
                texture_key,
                Vector2f(character.x, character.y),
                Vector2f(0.0f, 0.0f),
                Vector2f(1.0f, 1.0f),
                sample_rectangle.position,
                sample_rectangle.size,
                0.0f,
                _tint,
                _shader_key, 0.0f
            );
        }

        glDeleteFramebuffers(1, &frame_buffer_object);

        glBindFramebuffer(GL_FRAMEBUFFER ,_graphics->GetFrameBufferObject());

        //I realised that I need to delete the frame buffer object here, and there also is no point in storing it. I have to restructure this
        // to be a utility function or something instead of a class. No storing invalid FBOs!

    }

    glViewport(0,0,_engine->game_width, _engine->game_height);
    _graphics->SetProjection(0.0f, _engine->game_width,_engine->game_height, 0.0f);

    Vector2f partial_rendered_text_texture_size = Vector2f(frame_buffer_texture.width, frame_buffer_texture.height);
    if(_rectangle.size.x < partial_rendered_text_texture_size.x) partial_rendered_text_texture_size.x = _rectangle.size.x;
    if(_rectangle.size.y < partial_rendered_text_texture_size.y) partial_rendered_text_texture_size.y = _rectangle.size.y;

    //This is first when we know if a scrollbar is even needed or not. So toggling the scrollbar here would be appropriate

    _parent->contents_to_window_ratio_x = partial_rendered_text_texture_size.x/_rectangle.size.x;
    _parent->contents_to_window_ratio_y = partial_rendered_text_texture_size.y/_rectangle.size.y;

    Console::PrintLine(_parent->contents_to_window_ratio_x, _parent->contents_to_window_ratio_y);

    _graphics->DrawPartialSprite(frame_buffer_texture,
        Vector2f(_position.x, _position.y),
        Vector2f(0.0f, 0.0f),
        _scale,
        _rectangle.position,
        partial_rendered_text_texture_size,
        0.0f,
        _tint,
        _shader_key,
        0.0f
    );

    //...
}

}
