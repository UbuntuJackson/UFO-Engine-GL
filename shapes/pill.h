#ifndef PILL_H
#define PILL_H

#include "raw_shape_base.h"
#include "../external/olcPixelGameEngine.h"

class Pill : public RawShapeBase{
public:
    float height;
    float radius;
    Pill(olc::vf2d _position, float _radius, float _height);

    bool IsOverlapping(Circle* _circle);

    bool IsOverlapping(ufo::Rectangle* _circle);

    bool IsOverlappingOther(RawShapeBase* _other);

    //void Draw(Camera *_camera);

};

#endif