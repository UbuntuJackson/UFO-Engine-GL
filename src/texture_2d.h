#pragma once
#include "../glad/include/glad/glad.h"
#include "../utils/console.h"
#include "ufo_macros.h"
#include "../utils/opengl_utils.h"
#include "ufo_maths.h"
#include "../external/olcPixelGameEngine.h"

namespace ufo{

class Texture2D{
public:
    //Holds the ID of the texture object used for texture operations.
    unsigned int id = 0;
    unsigned int width;
    unsigned int height;
    unsigned int internal_format; //Format of texture object apparently?
    unsigned int image_format; //Format of loaded image?
    unsigned int wrapping_mode_s_axis;
    unsigned int wrapping_mode_t_axis;
    unsigned int filter_mode_min; //Filtering mode if texture pixles < screen pixels?
    unsigned int filter_mode_max; //Filtering mode if texture pixles > screen pixels?
    bool permanent = false; //If the asset is supposed to be preloaded and saved

    int number_of_colour_channels = 4;

    unsigned char* pixel_data;

    Texture2D();

    void Generate(unsigned int _width, unsigned int _height, unsigned char* _data);

    olc::Pixel GetPixel(Vector2i _position);

    void Bind();

    void Delete();

};

};
