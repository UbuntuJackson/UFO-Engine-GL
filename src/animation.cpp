#include <unordered_map>
#include <vector>
#include "../ufo_maths/ufo_maths.h"
#include "graphics.h"
#include "actor.h"
#include "animation.h"
#include "sprite.h"

Animation::Animation(Vector2f _local_position) : Sprite("?", _local_position, Vector2f(0.0f, 0.0f), Vector2f(16.0f, 16.0f), Vector2f(1.0f, 1.0f), 0.0f, 0.0f){

}

void Animation::SetSpriteConfiguration(const std::string& _configuration_key){
    SpriteConfiguration& configuration = configurations.at(_configuration_key);
    key = configuration._key;
    local_position = configuration._position;
    offset = configuration._offset;
    frame_size = configuration._frame_size;
    scale = configuration._scale;
    rotation = configuration._rotation;
    current_frame_index = configuration._frame_index;
}

void Animation::AddSpriteConfiguration(std::string _key, olc::vf2d _position, olc::vf2d _offset, olc::vf2d _frame_size, olc::vf2d _scale, float _rotation, float _frame_index){
    SpriteConfiguration configuration = {
        _key,
        _position,
        _offset,
        _frame_size,
        _scale,
        _rotation,
        _frame_index
    };

    configurations.emplace(_key, configuration);

}

void Animation::OnSpawn(){

}

void Animation::OnDraw(ufo::Graphics* _graphics, Camera* _camera){

    current_animation->OnDraw(_graphics, _camera);

}
