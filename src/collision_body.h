#pragma once
#include "actor.h"
#include "../ufo_maths/ufo_maths.h"

namespace ufo{

class Collision : public Actor{
public:

    Vector2f velocity;

    Collision(Vector2f _) : Actor(_){}

    void DetectAndAvoidWorldCollisionCircle(Vector2f& _position, const float _radius){

    }

    void DetectAndAvoidWorldCollisionRectangle(Vector2f& _position, const Vector2f _size){

    }

    void OverlapAndResolveRectangle(Vector2f& _position, const Vector2f _size){

    }

    void OnUpdate(float _delta_time){

    }

    void OnDraw(ufo::Graphics* _graphics, Camera* _camera){

    }

};

}
