#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "../external/olcPixelGameEngine.h"
#include "raw_shape_base.h"

class Circle;
class Camera;

namespace ufo{

class Rectangle : public RawShapeBase{
public:
    olc::vf2d size;
    Rectangle(olc::vf2d _position, olc::vf2d _size);
    bool IsOverlapping(Circle* _circle);
    bool IsOverlapping(Rectangle* _circle);
    bool IsOverlappingOther(RawShapeBase* _other);
    //void Draw(Camera *_camera);
};

}

#endif