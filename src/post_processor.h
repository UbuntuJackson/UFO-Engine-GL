#pragma once
#include "texture_2d.h"
#include "shader.h"

namespace ufo{

class PostProcessor{
public:
    Shader post_processing_shader;
    Texture2D texture;

    unsigned int width, height;

    bool shake = false;
    PostProcessor(Shader _shader, unsigned int _width, unsigned int _height);

    void BeginRender();

    void EndRender();

    void Render(float _time);
private:
    unsigned int multi_sampled_FBO, FBO;
    unsigned int RBO;
    unsigned int VAO;

    void InitialiseRenderData();



};

}