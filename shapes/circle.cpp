#include "../external/olcPixelGameEngine.h"
#include "circle.h"
#include "raw_shape_base.h"
#include "../ufo_maths/ufo_maths.h"

namespace ufo{

Circle::Circle(olc::vf2d _position, float _radius) :
RawShapeBase(_position),
    position{_position},
    radius{_radius}
    {}

bool Circle::IsOverlapping(Circle* _circle){
    return ufo::Maths::IsOverlapping(*this,*_circle);
}

bool Circle::IsOverlapping(ufo::Rectangle* _circle){
    return ufo::Maths::IsOverlapping(*this,*_circle);
}

bool Circle::IsOverlappingOther(RawShapeBase* _other){
    return _other->IsOverlapping(this);
}

}
//void Circle::Draw(Camera *_camera){
    //DrawingSystem::Draw(*this,_camera, colour);
//}
