#include "ray2.h"
#include "../external/olcPixelGameEngine.h"
Ray2::Ray2(olc::vf2d _p0, olc::vf2d _p1) : p0{_p0}, p1{_p1}{}

/*Ray2::Ray2(const Ray2& _copy){
    p0 = _copy.p0;
    p1 = _copy.p1;
}*/

olc::vf2d
Ray2::Start() const{
    return p0;
}

olc::vf2d
Ray2::End() const{
    return p1;
}

olc::vf2d
Ray2::Size() const{
    return p1 - p0;
}

olc::vf2d
Ray2::Normal() const{
    return olc::vf2d(Size().y, -Size().x);
}