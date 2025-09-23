#ifndef RAY_3_H
#define RAY_3_H
#include "../external/olcPixelGameEngine.h"
class Ray3{
public:
    olc::vf2d p1;
    olc::vf2d p2;
    olc::vf2d Start();
    olc::vf2d End();
    olc::vf2d Size();
    Ray2() = default;
    Ray2(olc::vf2d _p0, olc::vf2d _p1);
    olc::vf2d Normal();
};

#endif