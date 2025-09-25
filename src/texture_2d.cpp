#include "../glad/include/glad/glad.h"
#include "../utils/console.h"
#include "../utils/ufo_macros.h"
#include "../utils/opengl_utils.h"
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
    Console::PrintLine("Texture2D()",id);
}

void Texture2D::Generate(unsigned int _width, unsigned int _height, unsigned char* _data){

    glGenTextures(1, &id);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    width = _width;
    height = _height;

    //This creates the texture
    glBindTexture(GL_TEXTURE_2D, id);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);
    
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
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping_mode_s_axis);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping_mode_t_axis);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_mode_min);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_mode_max);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    //Resetting the currently bound texture
    glBindTexture(GL_TEXTURE_2D, 0);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);
}

void Texture2D::Bind(){
    //Console::PrintLine("Binding texture",id);
    glBindTexture(GL_TEXTURE_2D, id);
}

}