#ifndef RAY_2_H
#define RAY_2_H
#include "../external/olcPixelGameEngine.h"
class Ray2{
public:
    olc::vf2d p0;
    olc::vf2d p1;
    olc::vf2d Start() const;
    olc::vf2d End() const;
    //Ray2(const Ray2& _copy);
    olc::vf2d Size() const;
    Ray2() = default;
    Ray2(olc::vf2d _p0, olc::vf2d _p1);
    olc::vf2d Normal() const;
};

#endif