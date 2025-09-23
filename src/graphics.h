//Abstact graphics class

#pragma once
#include <string>
#include <glm/glm.hpp>
#include "../external/olcPixelGameEngine.h"
#include "../ufo_maths/ufo_maths.h"

namespace ufo{

typedef olc::Pixel Colour;

class Graphics{
public:
    Graphics() = default;

    Graphics(const Graphics&) = delete;
    Graphics(Graphics&&) = delete;

    virtual void SetProjection(float _left, float _right, float _bottom, float _top) = 0;

    virtual void DrawSprite(
        const std::string& _sprite_key,
        glm::vec2 _position,
        glm::vec2 _size,
        glm::vec2 _centre,
        glm::vec2 _v_scale,
        float _rotate,
        glm::vec3 _colour) = 0;

    virtual void DrawCircle(Vector2f _position, float _radius, ufo::Colour _colour) = 0;
    
    virtual void DrawRectangleExtra(Vector2f _position, Vector2f _size, Vector2f _centre, Vector2f _v_scale, float _rotation, ufo::Colour _colour) = 0;

    virtual void DrawPartialSprite(const std::string& _texture_key, Vector2f _position, Vector2f _size, Vector2f _centre, Vector2f _v_scale, Vector2f _sample_position, Vector2f _sample_size, float _rotation, ufo::Colour _colour) = 0;

    virtual ~Graphics() = default;

};

}