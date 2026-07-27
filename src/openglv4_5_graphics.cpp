#include "texture_2d.h"
#include "openglv4_5_graphics.h"
#include "shader.h"
#include "../glad/include/glad/glad.h"
#include "../utils/console.h"
#include "ufo_macros.h"
#include "openglv4_5_asset_manager.h"
#include <glm/gtx/string_cast.hpp>
#include "input.h"
#include <SDL3/SDL.h>
#include "engine.h"
#include "../ufo_maths/ufo_maths.h"

namespace ufo{

OpenGLv4_5_Graphics::OpenGLv4_5_Graphics(Engine* _engine) : engine{_engine}{

    InitialiseRenderData(_engine);

}

void OpenGLv4_5_Graphics::SetProjection(float _left, float _right, float _bottom, float _top){
    glm::mat4 projection = glm::ortho(
        _left, static_cast<float>(_right),
        static_cast<float>(_bottom), _top,
        -1.0f, 0.0f
    );

    for(auto& [name, shader] : engine->asset_manager.shaders){
        shader.Use();
        shader.SetInt("image", 0);
        shader.SetMatrix4("projection", projection);

    }


}

OpenGLv4_5_Graphics::~OpenGLv4_5_Graphics(){
    glDeleteVertexArrays(1, &quadVAO);
}

void OpenGLv4_5_Graphics::InitialiseRenderData(Engine* _engine){

    _engine->asset_manager.LoadShader(
        std::string(_engine->engine_path+"/shaders/sprite_transformation_vertex_shader.cs").c_str(),
        std::string(_engine->engine_path+"/shaders/sprite_transformation_fragment_shader.cs").c_str(),
        nullptr, "sprite_shader");

    sprite_shader = _engine->asset_manager.GetShader("sprite_shader");
    _engine->asset_manager.GetShader("sprite_shader").permanent = true;

    _engine->asset_manager.LoadShader(
        std::string(_engine->engine_path+"/shaders/partial_sprite_vertex.cs").c_str(),
        std::string(_engine->engine_path+"/shaders/partial_sprite_fragment.cs").c_str(),
        nullptr, "partial_sprite_shader");

    partial_sprite_shader = _engine->asset_manager.GetShader("partial_sprite_shader");
    _engine->asset_manager.GetShader("partial_sprite_shader").permanent = true;

    _engine->asset_manager.LoadShader(
        std::string(_engine->engine_path+"/shaders/rounded_corners_shader/partial_sprite_vertex.cs").c_str(),
        std::string(_engine->engine_path+"/shaders/rounded_corners_shader/partial_sprite_fragment.cs").c_str(),
        nullptr, "rounded_corners_shader");

    rounded_corners_shader = _engine->asset_manager.GetShader("rounded_corners_shader");
    _engine->asset_manager.GetShader("rounded_corners_shader").permanent = true;

    _engine->asset_manager.LoadShader(
        std::string(_engine->engine_path+"/shaders/rectangle_vertex_shader.cs").c_str(),
        std::string(_engine->engine_path+"/shaders/rectangle_fragment_shader.cs").c_str(),
        nullptr, "rectangle_shader");

    rectangle_shader = _engine->asset_manager.GetShader("rectangle_shader");
    _engine->asset_manager.GetShader("rectangle_shader").permanent = true;

    _engine->asset_manager.LoadShader(
        std::string(_engine->engine_path+"/shaders/circle_vertex_shader.cs").c_str(),
        std::string(_engine->engine_path+"/shaders/circle_fragment_shader.cs").c_str(),
        nullptr, "circle_shader");

    circle_shader = _engine->asset_manager.GetShader("circle_shader");
    _engine->asset_manager.GetShader("circle_shader").permanent = true;

    glm::mat4 projection = glm::ortho(
        0.0f, static_cast<float>(_engine->width),
        static_cast<float>(engine->height), 0.0f,
        -1.0f, 0.0f
    );

    Console::PrintLine("Shader program id:",sprite_shader.shader_program_id);

    _engine->asset_manager.LoadTexture(_engine->engine_path+"/res/face.png", "face", true);

    circle_shader.Use();
    circle_shader.SetMatrix4("projection", projection);


    rectangle_shader.Use();
    rectangle_shader.SetInt("image", 0);
    rectangle_shader.SetMatrix4("projection", projection);

    sprite_shader.Use();
    sprite_shader.SetInt("image", 0);
    sprite_shader.SetMatrix4("projection", projection);

    partial_sprite_shader.Use();
    partial_sprite_shader.SetInt("image", 0);
    partial_sprite_shader.SetMatrix4("projection", projection);

    rounded_corners_shader.Use();
    rounded_corners_shader.SetInt("image", 0);
    rounded_corners_shader.SetMatrix4("projection", projection);

    unsigned int VBO;
    /*float verticies[] = {
        //position  //texture
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };*/

    float verticies[] = {
        //position  //texture
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &quadVAO);


    glGenBuffers(1, &VBO);


    glBindBuffer(GL_ARRAY_BUFFER, VBO);


    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);


    glBindVertexArray(quadVAO);


    glEnableVertexAttribArray(0);


    glVertexAttribPointer(0,4,GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);


    glBindBuffer(GL_ARRAY_BUFFER,0);


    glBindVertexArray(0);

}

void OpenGLv4_5_Graphics::DrawSprite(
    const std::string& _texture_key, glm::vec2 _position, glm::vec2 _size, glm::vec2 _centre, glm::vec2 _v_scale, float _rotation, glm::vec3 _colour
){

    sprite_shader.Use();

    //Are these all ones?
    glm::mat4 model = glm::mat4(1.0f);
    //Moving the identity matrix to _position
    model = glm::translate(model, glm::vec3(_position, 0.0f));

    model = glm::scale(model, glm::vec3(_v_scale, 1.0f));
    model = glm::translate(model, glm::vec3(_centre.x, _centre.y, 0.0f));

    model = glm::rotate(model, glm::radians(_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-_centre.x, -_centre.y, 0.0f));

    model = glm::scale(model, glm::vec3(_size, 1.0f));

    sprite_shader.SetMatrix4("model", model);
    sprite_shader.SetVector3f("spriteColor", _colour);

    glActiveTexture(GL_TEXTURE0);
    //

    engine->asset_manager.textures.at(_texture_key).Bind();

    glBindVertexArray(quadVAO);
    //
    glDrawArrays(GL_TRIANGLES, 0, 6);
    //
    glBindVertexArray(0);
    //
}

void OpenGLv4_5_Graphics::glm_DrawCircle(glm::vec2 _position, float _radius, glm::vec4 _colour){
    circle_shader.Use();

    //Are these all ones?
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(_position-glm::vec2(_radius, _radius), 0.0f));

    //Moving the identity matrix to _position
    model = glm::scale(model, glm::vec3(_radius*2.0f, _radius*2.0f, 0.0f));

    circle_shader.SetMatrix4("model", model);
    circle_shader.SetVector4f("spriteColor", _colour);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void OpenGLv4_5_Graphics::DrawCircle(Vector2f _position, float _radius, ufo::Colour _colour){
    glm_DrawCircle(glm::vec2(_position.x, _position.y), _radius, glm::vec4(_colour.r/255.0f, _colour.g/255.0f, _colour.b/255.0f, _colour.a/255.0f));
}

void OpenGLv4_5_Graphics::glm_DrawRectangleExtra(glm::vec2 _position, glm::vec2 _size, glm::vec2 _centre, glm::vec2 _v_scale, float _rotation, glm::vec4 _colour, float _rounding){

    unsigned int VBO;

    float verticies[] = {
        //position  //texture
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenBuffers(1, &VBO);


    glBindBuffer(GL_ARRAY_BUFFER, VBO);


    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);


    glBindVertexArray(quadVAO);


    glEnableVertexAttribArray(0);


    glVertexAttribPointer(0,4,GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);


    glBindBuffer(GL_ARRAY_BUFFER,0);


    glBindVertexArray(0);


    rectangle_shader.Use();

    //Are these all ones?
    glm::mat4 model = glm::mat4(1.0f);
    //Moving the identity matrix to _position
    model = glm::translate(model, glm::vec3(_position, 0.0f));

    model = glm::scale(model, glm::vec3(_v_scale, 1.0f));
    model = glm::translate(model, glm::vec3(_centre.x, _centre.y, 0.0f));

    model = glm::rotate(model, glm::radians(_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-_centre.x, -_centre.y, 0.0f));

    model = glm::scale(model, glm::vec3(_size, 1.0f));

    rectangle_shader.SetMatrix4("model", model);
    rectangle_shader.SetVector4f("spriteColor", _colour);
    rectangle_shader.SetFloat("corner_rounding", _rounding);
    rectangle_shader.SetVector2f("size_not_normalised", _size);

    glActiveTexture(GL_TEXTURE0);
    //

    //_texture.Bind();

    glBindVertexArray(quadVAO);
    //
    glDrawArrays(GL_TRIANGLES, 0, 6);
    //
    glBindVertexArray(0);
    //

    glDeleteBuffers(1, &VBO);

}

void OpenGLv4_5_Graphics::DrawRectangleExtra(Vector2f _position, Vector2f _size, Vector2f _centre, Vector2f _v_scale, float _rotation, ufo::Colour _colour, float _rounding){

    glm_DrawRectangleExtra(glm::vec2(_position.x, _position.y), glm::vec2(_size.x, _size.y), glm::vec2(_centre.x, _centre.y), glm::vec2(_v_scale.x, _v_scale.y), _rotation, glm::vec4(_colour.r/255.0f, _colour.g/255.0f, _colour.b/255.0f, _colour.a/255.0f), _rounding);

}

void OpenGLv4_5_Graphics::glm_DrawPartialSprite(ufo::Texture2D& _texture, glm::vec2 _position, glm::vec2 _centre, glm::vec2 _v_scale, glm::vec2 _sample_position, glm::vec2 _sample_size, float _rotation, glm::vec4 _colour, const std::string& _shader ,float _corner_rounding){

    ufo::Shader& local_partial_sprite_shader = engine->asset_manager.GetShader(_shader);

    //Change vertecies

    unsigned int VBO;

    unsigned int texture_width = _texture.width;
    unsigned int texture_height = _texture.height;

    glm::vec2 size = glm::vec2((float)texture_width, (float)texture_height);

    glm::vec2 sample_size_normalised = _sample_size/size;

    glm::vec2 sample_position_normalised = _sample_position/size;

    float verticies[] = {
        //position                                          //texture
        0.0f, sample_size_normalised.y,                     0.0f, sample_size_normalised.y,
        sample_size_normalised.x, 0.0f,                     sample_size_normalised.x, 0.0f,
        0.0f, 0.0f,                                         0.0f, 0.0f,

        0.0f, sample_size_normalised.y,                     0.0f, sample_size_normalised.y,
        sample_size_normalised.x, sample_size_normalised.y, sample_size_normalised.x, sample_size_normalised.y,
        sample_size_normalised.x, 0.0f,                     sample_size_normalised.x, 0.0f
    };

    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

    glBindVertexArray(quadVAO);

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0,4,GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER,0);

    glBindVertexArray(0);

    local_partial_sprite_shader.Use();

    //The model is for like, one point.
    //It is then applied to all points with the sample size and position
    glm::mat4 model = glm::mat4(1.0f);
    //Moving the identity matrix to _position
    model = glm::translate(model, glm::vec3(_position, 0.0f));

    model = glm::scale(model, glm::vec3(_v_scale, 1.0f));

    model = glm::rotate(model, glm::radians(_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-_centre.x, -_centre.y, 0.0f));

    model = glm::scale(model, glm::vec3(size, 1.0f));

    local_partial_sprite_shader.SetMatrix4("model",                       model);
    local_partial_sprite_shader.SetVector4f("spriteColor",                _colour);
    local_partial_sprite_shader.SetVector2f("sample_position",            sample_position_normalised);
    local_partial_sprite_shader.SetVector2f("sample_size_not_normalised", _sample_size);
    local_partial_sprite_shader.SetFloat(   "corner_rounding",            _corner_rounding);
    local_partial_sprite_shader.SetVector2f("texture_size", glm::vec2(_texture.width, _texture.height));

    glActiveTexture(GL_TEXTURE0);
    //

    _texture.Bind();

    glBindVertexArray(quadVAO);
    //
    glDrawArrays(GL_TRIANGLES, 0, 6);
    //
    glBindVertexArray(0);
    //

    glDeleteBuffers(1, &VBO);

}

void OpenGLv4_5_Graphics::DrawPartialSprite(ufo::Texture2D& _texture, Vector2f _position, Vector2f _centre, Vector2f _v_scale, Vector2f _sample_position, Vector2f _sample_size, float _rotation, ufo::Colour _colour, const std::string& _shader,float _corner_rounding){

    glm_DrawPartialSprite(_texture, glm::vec2(_position.x, _position.y), glm::vec2(_centre.x, _centre.y), glm::vec2(_v_scale.x, _v_scale.y), glm::vec2(_sample_position.x, _sample_position.y) ,glm::vec2(_sample_size.x, _sample_size.y), _rotation, glm::vec4(_colour.r/255.0f, _colour.g/255.0f, _colour.b/255.0f, _colour.a/255.0f),_shader, _corner_rounding);

}

void OpenGLv4_5_Graphics::DrawPartialSprite(const std::string& _texture_key, Vector2f _position, Vector2f _centre, Vector2f _v_scale, Vector2f _sample_position, Vector2f _sample_size, float _rotation, ufo::Colour _colour, const std::string& _shader, float _corner_rounding){

    glm_DrawPartialSprite(engine->asset_manager.textures.at(_texture_key), glm::vec2(_position.x, _position.y), glm::vec2(_centre.x, _centre.y), glm::vec2(_v_scale.x, _v_scale.y), glm::vec2(_sample_position.x, _sample_position.y) ,glm::vec2(_sample_size.x, _sample_size.y), _rotation, glm::vec4(_colour.r/255.0f, _colour.g/255.0f, _colour.b/255.0f, _colour.a/255.0f), _shader, _corner_rounding);

}


void OpenGLv4_5_Graphics::CreateFrameBuffer(){
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (uint32_t)engine->width, (uint32_t)engine->height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id,0);

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
        (uint32_t)engine->width, (uint32_t)engine->height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        Console::PrintLine("Error, framebuffer no good");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    Console::PrintLine("OpenGLv4_5_Graphics::CreateFrameBuffer: Framebuffer created.");
}

void OpenGLv4_5_Graphics::BindFrameBuffer(){
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    //Console::PrintLine(FBO, RBO);
}

void OpenGLv4_5_Graphics::UnbindFrameBuffer(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLv4_5_Graphics::RescaleFrameBuffer(int _width, int _height){
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id,0);

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
        _width, _height);

    glDeleteRenderbuffers(1, &RBO);
}

}
