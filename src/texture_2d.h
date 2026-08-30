#pragma once
#include "../glad/include/glad/glad.h"
#include "../ufo_maths/ufo_maths.h"
#include "../external/olcPixelGameEngine.h"

namespace ufo{

//TODO: INvestigate if constructor needs to be virtual, probably doesn't
class Texture2D{
public:
    //Holds the ID of the texture object used for texture operations.
    unsigned int id = 0;
    unsigned int width;
    unsigned int height;
    unsigned int internal_format = GL_RGBA; //Format of texture object apparently?
    unsigned int image_format = GL_RGBA; //Format of loaded image?
    unsigned int wrapping_mode_s_axis = GL_REPEAT;
    unsigned int wrapping_mode_t_axis = GL_REPEAT;
    unsigned int filter_mode_min = GL_NEAREST; //Filtering mode if texture pixles < screen pixels?
    unsigned int filter_mode_max = GL_NEAREST; //Filtering mode if texture pixles > screen pixels?

    //To be deprecated
    bool is_savable = false; //If the asset is supposed to be preloaded and saved
    void Update();

    bool is_global_asset = true;

    int number_of_colour_channels = 4;

    unsigned char* pixel_data;

    Vector2i Size();

    Texture2D();

    void Generate(unsigned int _width, unsigned int _height, unsigned char* _data);

    olc::Pixel GetPixel(Vector2i _position);
    olc::Pixel GetPixel(int _x, int _y);
    void SetPixel(Vector2i _position, const olc::Pixel& _colour);
    void SetPixel(int _x, int _y, const olc::Pixel& _colour);

    void Bind();

    void Delete();

};

};
