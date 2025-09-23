#ifndef RAW_SHAPE_BASE
#define RAW_SHAPE_BASE

#include "../external/olcPixelGameEngine.h"

class Camera;
namespace ufo{class Rectangle;}
class Circle;
class Pill;

class RawShapeBase{
public:
    olc::Pixel colour = olc::VERY_DARK_BLUE;
    olc::vf2d position;
    RawShapeBase(olc::vf2d _position);
    virtual void Update();
    virtual void Draw(Camera* _camera);
    virtual bool IsOverlappingOther(RawShapeBase* _other);
    virtual bool IsOverlapping(Circle* _circle);
    virtual bool IsOverlapping(ufo::Rectangle* _circle);
    virtual bool IsOverlapping(Pill* _pill);
};

#endif