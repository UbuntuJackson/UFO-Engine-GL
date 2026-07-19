#include "../glad/include/glad/glad.h"
#include "../utils/console.h"
#include "ufo_macros.h"
#include "texture_2d.h"

namespace ufo{

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

void Texture2D::Generate(unsigned int _width, unsigned int _height, unsigned char* _data){

    //std::memcpy(&pixel_data, _data, sizeof(unsigned char) * _width * _height * 4);

    glGenTextures(1, &id);


    width = _width;
    height = _height;

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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping_mode_s_axis);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping_mode_t_axis);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_mode_min);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_mode_max);

    //Resetting the currently bound texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

olc::Pixel Texture2D::GetPixel(Vector2i _position){
    int i = _position.x*number_of_colour_channels+width*number_of_colour_channels*_position.y;
    return olc::Pixel(pixel_data[i],pixel_data[i+1],pixel_data[i+2],pixel_data[i+3]);
}

void Texture2D::Bind(){
    //Console::PrintLine("Binding texture",id);
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture2D::Delete(){
    glDeleteTextures(1, &id);
}

}
