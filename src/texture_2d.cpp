#include "../glad/include/glad/glad.h"
#include "../utils/console.h"
#include "ufo_macros.h"
#include "ufo_maths.h"
#include "texture_2d.h"
#include "graphics.h"

namespace ufo{

Vector2i Texture2D::Size(){
    return Vector2i(width, height);
}

Texture2D::Texture2D() :
    width{0},
    height{0},
    internal_format{GL_RGB},
    image_format{GL_RGB},
    wrapping_mode_s_axis{GL_REPEAT},
    wrapping_mode_t_axis{GL_REPEAT},
    filter_mode_min{GL_NEAREST},
    filter_mode_max{GL_NEAREST}
{

}

void Texture2D::Update(){

    glTexImage2D(
        GL_TEXTURE_2D,

        //This is referred to as 'level' but I do not know what it means
        0,

        //Some kind of internal format.
        internal_format,

        width,
        height,

        //Border?
        0,

        //
        image_format,

        //
        GL_UNSIGNED_BYTE,

        //This is the actual image data
        pixel_data
    );

}

void Texture2D::Generate(unsigned int _width, unsigned int _height, unsigned char* _data){

    //std::memcpy(&pixel_data, _data, sizeof(unsigned char) * _width * _height * 4);

    glGenTextures(1, &id);


    width = _width;
    height = _height;

    pixel_data = (unsigned char*)std::malloc(width*height*4);

    //This creates the texture
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(
        GL_TEXTURE_2D,

        //This is referred to as 'level' but I do not know what it means
        0,

        //Some kind of internal format.
        internal_format,

        width,
        height,

        //Border?
        0,

        //
        image_format,

        //
        GL_UNSIGNED_BYTE,

        //This is the actual image data
        _data
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_mode_min);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_mode_max);

    if(!pixel_data) Console::PrintLine(__UFO_PRETTY_FUNCTION__,"Error, failed to allocate pixel data");
    else{
        glGetTexImage(
            GL_TEXTURE_2D,
            0,
            image_format,
            GL_UNSIGNED_BYTE, pixel_data);
    }

    //Resetting the currently bound texture
    glBindTexture(GL_TEXTURE_2D, 0);

}

olc::Pixel Texture2D::GetPixel(Vector2i _position){
    if(_position.x < 0 || _position.x > width || _position.y < 0 || _position.y > height) return olc::Pixel(0,0,0,0);
    int i = _position.x*number_of_colour_channels+width*number_of_colour_channels*_position.y;
    return olc::Pixel(pixel_data[i],pixel_data[i+1],pixel_data[i+2],pixel_data[i+3]);
}

olc::Pixel Texture2D::GetPixel(int _x, int _y){

    return GetPixel(Vector2i(_x, _y));
}

void Texture2D::SetPixel(Vector2i _position, const olc::Pixel& _colour){
    if(_position.x < 0 || _position.x > width || _position.y < 0 || _position.y > height) return;
    int colour_start = _position.x*4+width*4*(_position.y);
    pixel_data[colour_start+0] = _colour.r;
    pixel_data[colour_start+1] = _colour.g;
    pixel_data[colour_start+2] = _colour.b;
    pixel_data[colour_start+3] = _colour.a;
}

void Texture2D::SetPixel(int _x, int _y,const olc::Pixel& _colour){
    SetPixel(Vector2i(_x, _y), _colour);
}

void Texture2D::Bind(){
    //Console::PrintLine("Binding texture",id);
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture2D::Delete(){
    std::free(pixel_data);
    glDeleteTextures(1, &id);
}

}
