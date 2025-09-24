#pragma once
#include "../ufo_maths/ufo_maths.h"
#include "actor.h"
#include "engine.h"
#include "camera.h"
#include "../../shapes/rectangle.h"

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

    Sprite(std::string _key, olc::vf2d _position, olc::vf2d _offset, olc::vf2d _frame_size, olc::vf2d _scale, float _rotation) :
    key{_key},
    Actor(_position),
    offset{_offset},
    frame_size{_frame_size},
    scale{_scale},
    rotation{_rotation}
    {
        
    }

    ufo::Rectangle
    GetRectangle(int _x, int _y, Vector2f _frame_size){
        ufo::Rectangle rect = ufo::Rectangle({(float)(_x * _frame_size.x), (float)(_y * _frame_size.y)}, _frame_size);
        return rect;
    }

    ufo::Rectangle
    GetFrameFromSpriteSheet(std::string _sprite_key, int _frame, Vector2f _frame_size){
        return GetRectangle(
            (int)_frame % (engine->asset_manager.textures.at(_sprite_key).width/(int)_frame_size.x), //1 can only give me x = 0
            (int)_frame / (engine->asset_manager.textures.at(_sprite_key).width/(int)_frame_size.x),
            _frame_size); //1 can only give y = 1
    }

    void OnDraw(ufo::Graphics* _graphics, Camera* _camera){

        ufo::Rectangle sample_rectangle = GetFrameFromSpriteSheet(key,current_frame_index,frame_size);
        _graphics->DrawPartialSprite(
            key,
            _camera->Transform(GetGlobalPosition()),
            /*size,*/
            offset,
            scale*_camera->scale,
            sample_rectangle.position,
            sample_rectangle.size,
            rotation,
            tint
        );
    }
};