#include "pill.h"
#include "raw_shape_base.h"
#include "../external/olcPixelGameEngine.h"
#include "../ufo_maths/ufo_maths.h"

Pill::Pill(olc::vf2d _position, float _radius, float _height) : RawShapeBase(_position), radius{_radius}, height{_height}{}

bool Pill::IsOverlapping(Circle* _circle){
    return ufoMaths::IsOverlapping(*this,*_circle);
}

bool Pill::IsOverlapping(ufo::Rectangle* _circle){
    return ufoMaths::IsOverlapping(*this,*_circle);
}

bool Pill::IsOverlappingOther(RawShapeBase* _other){
    return _other->IsOverlapping(this);
}

/*void Pill::Draw(Camera *_camera){
    DrawingSystem::Draw(*this,_camera, colour);
}*/