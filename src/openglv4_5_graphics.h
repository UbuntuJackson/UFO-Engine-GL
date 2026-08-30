#pragma once
#include <string>
#include <glm/glm.hpp>
#include "shader.h"
#include "graphics.h"
#include "../ufo_maths/ufo_maths.h"
#include "texture_2d.h"

namespace ufo{

class Texture2D;
class Engine;

class OpenGLv4_5_Graphics : public Graphics{
private:
    Shader sprite_shader;
    Shader partial_sprite_shader;
    Shader rounded_corners_shader;
    Shader rectangle_shader;
    Shader circle_shader;
    Engine* engine;

    //Could I use this one to draw shapes too? Keep in mind I need a camera
    unsigned int quadVAO;

public:
    OpenGLv4_5_Graphics() = default;
    OpenGLv4_5_Graphics(Engine* _engine);
    virtual ~OpenGLv4_5_Graphics();

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

    void glm_DrawRectangleExtra(glm::vec2 _position, glm::vec2 _size, glm::vec2 _centre, glm::vec2 _v_scale, float _rotation, glm::vec4 _colour, float _rounding);
    void DrawRectangleExtra(Vector2f _position, Vector2f _size, Vector2f _centre, Vector2f _v_scale, float _rotation, ufo::Colour _colour, float _rounding);

    void glm_DrawPartialSprite(ufo::Texture2D& _texture, glm::vec2 _position,  glm::vec2 _centre, glm::vec2 _v_scale, glm::vec2 _sample_position, glm::vec2 _sample_size, float _rotation, glm::vec4 _colour, const std::string& _shader, float _corner_rounding);
    void DrawPartialSprite(const std::string& _texture_key, Vector2f _position, Vector2f _centre, Vector2f _v_scale, Vector2f _sample_position, Vector2f _sample_size, float _rotation, ufo::Colour _colour, const std::string& _shader, float _corner_rounding);
    void DrawPartialSprite(ufo::Texture2D& _texture, Vector2f _position, Vector2f _centre, Vector2f _v_scale, Vector2f _sample_position, Vector2f _sample_size, float _rotation, ufo::Colour _colour, const std::string& _shader, float _corner_rounding);

    void InitialiseRenderData(Engine* _engine);

    //To do: Free this
    unsigned int frame_buffer_object;
    unsigned int render_buffer_object;
    GLuint texture_id; //For framebuffer

    void SetFrameBufferObject(int _fbo) override;
    int GetFrameBufferObject() override;

    void CreateFrameBuffer();

    void BindFrameBuffer();

    void UnbindFrameBuffer();

    void RescaleFrameBuffer(int _width, int _height);
};

}
