#ifndef CIRCLE_H
#define CIRCLE_H

#include "raw_shape_base.h"
#include "../external/olcPixelGameEngine.h"

namespace ufo{class Rectangle;}
class Camera;

class Circle : public RawShapeBase{
public:
    olc::vf2d position;
    float radius;
    Circle(olc::vf2d _position, float _radius);
    bool IsOverlapping(Circle* _circle);
    bool IsOverlapping(ufo::Rectangle* _circle);
    bool IsOverlappingOther(RawShapeBase* _other);
    //void Draw(Camera *_camera);
};

#endif