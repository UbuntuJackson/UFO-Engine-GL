#include "../external/olcPixelGameEngine.h"
#include "circle.h"
#include "raw_shape_base.h"
#include "../ufo_maths/ufo_maths.h"
//#include "../drawing_system/drawing_system.h"
//#include "../camera/camera.h"

Circle::Circle(olc::vf2d _position, float _radius) :
RawShapeBase(_position),
    position{_position},
    radius{_radius}
    {}

bool Circle::IsOverlapping(Circle* _circle){
    return ufoMaths::IsOverlapping(*this,*_circle);
}

bool Circle::IsOverlapping(ufo::Rectangle* _circle){
    return ufoMaths::IsOverlapping(*this,*_circle);
}

bool Circle::IsOverlappingOther(RawShapeBase* _other){
    return _other->IsOverlapping(this);
}

//void Circle::Draw(Camera *_camera){
    //DrawingSystem::Draw(*this,_camera, colour);
//}