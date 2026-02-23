#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "../external/olcPixelGameEngine.h"
#include "raw_shape_base.h"

namespace ufo{

class Circle;
class Camera;

class Rectangle : public RawShapeBase{
public:
    olc::vf2d size;
    Rectangle(olc::vf2d _position, olc::vf2d _size);
    bool IsOverlapping(Circle* _circle);
    bool IsOverlapping(Rectangle* _circle);
    bool IsOverlappingOther(RawShapeBase* _other);
    //void Draw(Camera *_camera);
};

class RectangleInt{
    olc::vi2d size;
    RectangleInt(olc::vi2d _position, olc::vi2d _size);
};

}

#endif
