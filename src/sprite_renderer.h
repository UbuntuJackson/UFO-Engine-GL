#pragma once
#include <string>
#include <glm/glm.hpp>
#include "shader.h"
#include "graphics.h"
#include "../ufo_maths/ufo_maths.h"

namespace ufo{

class Texture2D;
class Engine;

class OpenGLv4_5_Graphics : public Graphics{
private:
    Shader shader;
    Shader partial_sprite_shader;
    Shader rectangle_shader;
    Shader circle_shader;
    Engine* engine;

    //Could I use this one to draw shapes too? Keep in mind I need a camera
    unsigned int quadVAO;

public:
    OpenGLv4_5_Graphics() = default;
    OpenGLv4_5_Graphics(Engine* _engine);
    ~OpenGLv4_5_Graphics();

    void SetProjection(float _left, float _right, float _bottom, float _top);

    void DrawSprite(
        const std::string& _texture_key,
        glm::vec2 _position,
        glm::vec2 _size,
        glm::vec2 _centre,
        glm::vec2 _v_scale,
        float _rotate,
        glm::vec3 _colour);
    
    void glm_DrawCircle(glm::vec2 _position, float _radius, glm::vec4 _colour);
    void DrawCircle(Vector2f _position, float _radius, ufo::Colour _colour);

    void glm_DrawRectangleExtra(glm::vec2 _position, glm::vec2 _size, glm::vec2 _centre, glm::vec2 _v_scale, float _rotation, glm::vec4 _colour);
    void DrawRectangleExtra(Vector2f _position, Vector2f _size, Vector2f _centre, Vector2f _v_scale, float _rotation, ufo::Colour _colour);

    void glm_DrawPartialSprite(const std::string& _texture_key, glm::vec2 _position,  glm::vec2 _centre, glm::vec2 _v_scale, glm::vec2 _sample_position, glm::vec2 _sample_size, float _rotation, glm::vec4 _colour);
    void DrawPartialSprite(const std::string& _texture_key, Vector2f _position, Vector2f _centre, Vector2f _v_scale, Vector2f _sample_position, Vector2f _sample_size, float _rotation, ufo::Colour _colour);

    void InitialiseRenderData();
};

}