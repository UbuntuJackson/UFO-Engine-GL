//Abstact graphics class

#pragma once
#include <string>
#include <glm/glm.hpp>
#include "../src/graphics.h"
#include "../src/engine.h"
#include "pixel_game_engine.h"
namespace ufo{

class PixelGameEngineGraphics : public Graphics{
public:
    PixelGameEngine* pixel_game_engine = nullptr;
    Engine* engine = nullptr;

    PixelGameEngineGraphics(Engine* _engine, PixelGameEngine* _pixel_game_engine) : engine{_engine},  pixel_game_engine{_pixel_game_engine}{}

    PixelGameEngineGraphics(const PixelGameEngineGraphics&) = delete;
    PixelGameEngineGraphics(PixelGameEngineGraphics&&) = delete;

    void DrawSprite(
        const std::string& _sprite_key,
        glm::vec2 _position,
        glm::vec2 _size,
        glm::vec2 _centre,
        glm::vec2 _v_scale,
        float _rotate,
    glm::vec3 _colour){

    }

    void DrawCircle(glm::vec2 _position, float _radius, glm::vec4 _colour){

    }

    void DrawRectangleExtra(Vector2f _position, Vector2f _size, Vector2f _centre, Vector2f _v_scale, float _rotation, Colour _colour){
        Console::Print("Error, function 'DrawRectangleExtra' not supported by PixelGameEngine\n");
    }

    ~PixelGameEngineGraphics(){}

};

}