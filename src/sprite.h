#pragma once
#include <string>
#include "../ufo_maths/ufo_maths.h"
#include "actor.h"
#include "graphics.h"

class Camera;

namespace ufo{
    class Graphics;
}

class Sprite : public Actor{
public:
    std::string key;
    Vector2f offset;
    Vector2f frame_size;
    Vector2f scale = {1.0f, 1.0f};
    float rotation = 0.0f;
    float current_frame_index = 0;
    int number_of_frames;
    bool visible = true;
    ufo::Colour tint = olc::WHITE;

    Sprite(std::string _key, olc::vf2d _position, olc::vf2d _offset, olc::vf2d _frame_size, olc::vf2d _scale, float _rotation, float _frame_index);

    ufo::Rectangle
    GetRectangle(int _x, int _y, Vector2f _frame_size);

    ufo::Rectangle
    GetFrameFromSpriteSheet(std::string _sprite_key, int _frame, Vector2f _frame_size);

    void OnDraw(ufo::Graphics* _graphics, Camera* _camera);
};
