#pragma once

#include <string>
#include "../external/olcPixelGameEngine.h"
#include "actor.h"

struct Bounds{
    float x0;
    float x1;
    float y0;
    float y1;
    float GetWidthHalf(){return (x1-x0)/2.0f;} //something is wrong if this returns negative
    float GetHeightHalf(){return (y1-y0)/2.0f;} //something is wrong if this returns negative
};

class Camera : public Actor{
public:
    bool clamp = false;
    int target_id = -1; //target looks through cameras and takes the id matching with it's own
    Bounds view;
    Bounds world;
    float rotation;
    float scale;
    olc::vf2d original_local_position;
    olc::vf2d offset_from_local_position;
    bool local_position_set_elsewhere = true;

    //For GL
    ufo::Rectangle viewport = ufo::Rectangle(Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0f));

    Camera(olc::vf2d _position);
    void OnSpawn();
    void EarlyUpdate();
    void ClampLocalPosition();
    void HandleUpdate();
    olc::vf2d Transform(const olc::vf2d& _position);
    olc::vf2d TransformScreenToWorld(const olc::vf2d& _screen_position);
    //SpriteReference Transform(const SpriteReference& _sprite);
    bool IsOnScreen(olc::vf2d _position, olc::vf2d _offset = {0.0f, 0.0f});
    ufo::Rectangle GetOnScreenRectangleInWorld(olc::vf2d _offset = {0.0f, 0.0f});
};