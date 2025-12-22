#pragma once
#include <unordered_map>
#include <vector>
#include "../ufo_maths/ufo_maths.h"
#include "graphics.h"
#include "sprite.h"

class Animation : public Sprite{
public:

    struct SpriteConfiguration{
        std::string _key;
        olc::vf2d _position;
        olc::vf2d _offset;
        olc::vf2d _frame_size;
        olc::vf2d _scale;
        float _rotation;
        float _frame_index;
    };

    //Make a special type of Sprite with overwritten Draw function to not have the OnDraw callback call by default
    Sprite* current_animation = nullptr;

    std::unordered_map<std::string, SpriteConfiguration> configurations;

    Animation(Vector2f _local_position);

    void SetSpriteConfiguration(const std::string& _configuration_key);

    void AddSpriteConfiguration(
        std::string _key,
        olc::vf2d _position,
        olc::vf2d _offset,
        olc::vf2d _frame_size,
        olc::vf2d _scale,
        float _rotation, float _frame_index);

    void OnSpawn();

    void OnDraw(ufo::Graphics* _graphics, Camera* _camera);
};
