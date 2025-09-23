#include "../external/olcPixelGameEngine.h"
#include "rectangle.h"
#include "raw_shape_base.h"
#include "../ufo_maths/ufo_maths.h"

namespace ufo{

Rectangle::Rectangle(olc::vf2d _position, olc::vf2d _size): RawShapeBase(_position), size{_size}{}

bool Rectangle::IsOverlapping(Circle* _circle){
    return ufoMaths::IsOverlapping(*this,*_circle);
}

bool Rectangle::IsOverlapping(Rectangle* _circle){
    return ufoMaths::IsOverlapping(*this,*_circle);
}

bool Rectangle::IsOverlappingOther(RawShapeBase* _other){
    return _other->IsOverlapping(this);
}

/*void Rectangle::Draw(Camera *_camera){
    DrawingSystem::Draw(*this,_camera, colour);
}*/

}