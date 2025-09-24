#include "texture_2d.h"
#include "sprite_renderer.h"
#include "shader.h"
#include "../glad/include/glad/glad.h"
#include "../utils/console.h"
#include "../utils/ufo_macros.h"
#include "openglv4_5_asset_manager.h"
#include "../utils/opengl_utils.h"
#include <glm/gtx/string_cast.hpp>
#include "input.h"
#include <SDL3/SDL.h>
#include "engine.h"
#include "../ufo_maths/ufo_maths.h"

namespace ufo{

OpenGLv4_5_Graphics::OpenGLv4_5_Graphics(Engine* _engine) : engine{_engine}{

    shader = _engine->asset_manager.LoadShader(
        std::string(_engine->engine_path+"/shaders/sprite_transformation_vertex_shader.cs").c_str(),
        std::string(_engine->engine_path+"/shaders/sprite_transformation_fragment_shader.cs").c_str(),
        nullptr, "sprite_shader");

    partial_sprite_shader = _engine->asset_manager.LoadShader(
        std::string(_engine->engine_path+"/shaders/partial_sprite_vertex.cs").c_str(),
        std::string(_engine->engine_path+"/shaders/partial_sprite_fragment.cs").c_str(),
        nullptr, "partial_sprite_shader");

    rectangle_shader = _engine->asset_manager.LoadShader(
        std::string(_engine->engine_path+"/shaders/rectangle_vertex_shader.cs").c_str(),
        std::string(_engine->engine_path+"/shaders/rectangle_fragment_shader.cs").c_str(),
        nullptr, "rectangle_shader");

    circle_shader = _engine->asset_manager.LoadShader(
        std::string(_engine->engine_path+"/shaders/circle_vertex_shader.cs").c_str(),
        std::string(_engine->engine_path+"/shaders/circle_fragment_shader.cs").c_str(),
        nullptr, "circle_shader");
    
    glm::mat4 projection = glm::ortho(
        0.0f, static_cast<float>(_engine->width),
        static_cast<float>(engine->height), 0.0f,
        -1.0f, 0.0f
    );

    Console::PrintLine("Shader program id:",shader.shader_program_id);
    
    _engine->asset_manager.LoadTexture(_engine->engine_path+"/res/face.png", "face", true);

    circle_shader.Use();
    circle_shader.SetMatrix4("projection", projection);
    

    rectangle_shader.Use();
    rectangle_shader.SetInt("image", 0);
    rectangle_shader.SetMatrix4("projection", projection);

    shader.Use();
    shader.SetInt("image", 0);
    shader.SetMatrix4("projection", projection);

    partial_sprite_shader.Use();
    partial_sprite_shader.SetInt("image", 0);
    partial_sprite_shader.SetMatrix4("projection", projection);

    InitialiseRenderData();

}

void OpenGLv4_5_Graphics::SetProjection(float _left, float _right, float _bottom, float _top){
    glm::mat4 projection = glm::ortho(
        0.0f, static_cast<float>(_right),
        static_cast<float>(_bottom), 0.0f,
        -1.0f, 0.0f
    );

    circle_shader.Use();
    circle_shader.SetMatrix4("projection", projection);
    

    rectangle_shader.Use();
    rectangle_shader.SetInt("image", 0);
    rectangle_shader.SetMatrix4("projection", projection);

    shader.Use();
    shader.SetInt("image", 0);
    shader.SetMatrix4("projection", projection);
}

OpenGLv4_5_Graphics::~OpenGLv4_5_Graphics(){
    glDeleteVertexArrays(1, &quadVAO);
}

void OpenGLv4_5_Graphics::InitialiseRenderData(){

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
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    glGenBuffers(1, &VBO);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    glBindVertexArray(quadVAO);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    glEnableVertexAttribArray(0);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    glVertexAttribPointer(0,4,GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);
    
    glBindBuffer(GL_ARRAY_BUFFER,0);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    glBindVertexArray(0);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);
}

void OpenGLv4_5_Graphics::DrawSprite(
    const std::string& _texture_key, glm::vec2 _position, glm::vec2 _size, glm::vec2 _centre, glm::vec2 _v_scale, float _rotation, glm::vec3 _colour
){

    shader.Use();

    //Are these all ones?
    glm::mat4 model = glm::mat4(1.0f);
    //Moving the identity matrix to _position
    model = glm::translate(model, glm::vec3(_position, 0.0f));

    model = glm::scale(model, glm::vec3(_v_scale, 1.0f));
    model = glm::translate(model, glm::vec3(_centre.x, _centre.y, 0.0f));
    
    model = glm::rotate(model, glm::radians(_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-_centre.x, -_centre.y, 0.0f));

    model = glm::scale(model, glm::vec3(_size, 1.0f));

    shader.SetMatrix4("model", model);
    shader.SetVector3f("spriteColor", _colour);

    glActiveTexture(GL_TEXTURE0);
    //GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    engine->asset_manager.textures.at(_texture_key).Bind();

    glBindVertexArray(quadVAO);
    //GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    //GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);
    glBindVertexArray(0);
    //GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);
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

void OpenGLv4_5_Graphics::glm_DrawRectangleExtra(glm::vec2 _position, glm::vec2 _size, glm::vec2 _centre, glm::vec2 _v_scale, float _rotation, glm::vec4 _colour){

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

    // I don't think I need to bind a texture here?
    //glActiveTexture(GL_TEXTURE0);
    //engine->asset_manager.textures.at("face").Bind();

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

}

void OpenGLv4_5_Graphics::DrawRectangleExtra(Vector2f _position, Vector2f _size, Vector2f _centre, Vector2f _v_scale, float _rotation, ufo::Colour _colour){

    glm_DrawRectangleExtra(glm::vec2(_position.x, _position.y), glm::vec2(_size.x, _size.y), glm::vec2(_centre.x, _centre.y), glm::vec2(_v_scale.x, _v_scale.y), _rotation, glm::vec4(_colour.r/255.0f, _colour.g/255.0f, _colour.b/255.0f, _colour.a/255.0f));

}

void OpenGLv4_5_Graphics::glm_DrawPartialSprite(const std::string& _texture_key, glm::vec2 _position, glm::vec2 _size, glm::vec2 _centre, glm::vec2 _v_scale, glm::vec2 _sample_position, glm::vec2 _sample_size, float _rotation, glm::vec4 _colour){

    //Change vertecies

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

    //unsigned int texture_width = _sample_size.x/engine->asset_manager.textures.at(_texture_key).width;
    //unsigned int texture_height = _sample_size.y/engine->asset_manager.textures.at(_texture_key).height;
    glm::vec2 sample_size_normalised = _sample_size/_size;

    glm::vec2 sample_position_normalised = _sample_position/_size;

    Console::PrintLine("Sample size normalised",sample_size_normalised.x, sample_size_normalised.y);

    float verticies[] = {
        //position  //texture
        0.0f, sample_size_normalised.y, 0.0f, sample_size_normalised.y,
        sample_size_normalised.x, 0.0f, sample_size_normalised.x, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        
        0.0f, sample_size_normalised.y, 0.0f, sample_size_normalised.y,
        sample_size_normalised.x, sample_size_normalised.y, sample_size_normalised.x, sample_size_normalised.y,
        sample_size_normalised.x, 0.0f, sample_size_normalised.x, 0.0f
    };

    glGenVertexArrays(1, &quadVAO);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    glGenBuffers(1, &VBO);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    glBindVertexArray(quadVAO);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    glEnableVertexAttribArray(0);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    glVertexAttribPointer(0,4,GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);
    
    glBindBuffer(GL_ARRAY_BUFFER,0);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    glBindVertexArray(0);
    GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    //Change vertecies

    partial_sprite_shader.Use();

    //Are these all ones?
    glm::mat4 model = glm::mat4(1.0f);
    //Moving the identity matrix to _position
    model = glm::translate(model, glm::vec3(_position, 0.0f));

    model = glm::scale(model, glm::vec3(_v_scale, 1.0f));
    model = glm::translate(model, glm::vec3(_centre.x, _centre.y, 0.0f));
    
    model = glm::rotate(model, glm::radians(_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-_centre.x, -_centre.y, 0.0f));

    model = glm::scale(model, glm::vec3(_size, 1.0f));

    partial_sprite_shader.SetMatrix4("model", model);
    partial_sprite_shader.SetVector3f("spriteColor", _colour);
    partial_sprite_shader.SetVector2f("sample_position", sample_position_normalised);
    partial_sprite_shader.SetVector2f("sample_size", sample_size_normalised);

    glActiveTexture(GL_TEXTURE0);
    //GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

    engine->asset_manager.textures.at(_texture_key).Bind();

    glBindVertexArray(quadVAO);
    //GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    //GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);
    glBindVertexArray(0);
    //GetGLError(__UFO_PRETTY_FUNCTION__, __LINE__);

}

void OpenGLv4_5_Graphics::DrawPartialSprite(const std::string& _texture_key, Vector2f _position, Vector2f _size, Vector2f _centre, Vector2f _v_scale, Vector2f _sample_position, Vector2f _sample_size, float _rotation, ufo::Colour _colour){

    glm_DrawPartialSprite(_texture_key, glm::vec2(_position.x, _position.y), glm::vec2(_size.x, _size.y), glm::vec2(_centre.x, _centre.y), glm::vec2(_v_scale.x, _v_scale.y), glm::vec2(_sample_position.x, _sample_position.y) ,glm::vec2(_sample_size.x, _sample_size.y), _rotation, glm::vec4(_colour.r/255.0f, _colour.g/255.0f, _colour.b/255.0f, _colour.a/255.0f));

}

}