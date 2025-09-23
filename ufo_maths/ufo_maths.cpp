#include <cmath>
#include <math.h>
#include "ufo_maths.h"
#include "../external/olcPixelGameEngine.h"
#include "../ufo_collision/collision_data.h"
#include "../shapes/ray2.h"
#include "../shapes/rectangle.h"
#include "../shapes/circle.h"
#include "../utils/console.h"
#include "../shapes/pill.h"
#include "cross_product.h"
#include "../shapes/vector3.h"
//#include "../shapes/shape.h"

namespace ufoMaths{

CollisionData RayVsRay(Ray2 _v, Ray2 _w){
    //Credits to egomoose for referring me to the 2d cross product.
    //The 2d crossproduct is actually just the z-value of the 3d cross product.

    olc::vf2d v_size = _v.End()-_v.Start();
    olc::vf2d w_size = _w.End()-_w.Start();
    Vector3 b = Vector3{v_size.x, v_size.y, 0.0f};
    Vector3 d = Vector3{w_size.x, w_size.y, 0.0f};
    Vector3 a = Vector3{_v.Start().x, _v.Start().y, 0.0f};
    Vector3 c = Vector3{_w.Start().x, _w.Start().y, 0.0f};
    float collision_time = CrossProduct(c-a, d).z/CrossProduct(b,d).z;
    float collision_time_other = CrossProduct(a-c, b).z/CrossProduct(d,b).z;

    //if(std::isinf(collision_time)) Console::Out("collision time is positive inf");
    //if(std::isnan(collision_time)) Console::Out("collision time is positive nan");
    //if(std::isinf(collision_time_other)) Console::Out("collision time other is positive inf");
    //if(std::isnan(collision_time_other)) Console::Out("collision time other is positive nan");

    olc::vf2d intersection_point = _v.Start() + _v.Size() * collision_time;
    
    bool hit_succeed = false;
    
    if((0.0f <= collision_time && collision_time <= 1.0f) && (0.0f <= collision_time_other && collision_time_other <= 1.0f)){
        hit_succeed = true;
    }
    if(std::isnan(collision_time)) hit_succeed = false;
    if(std::isinf(collision_time)) hit_succeed = false;
    if(std::isnan(collision_time_other)) hit_succeed = false;
    if(std::isinf(collision_time_other)) hit_succeed = false;

    return CollisionData{
        collision_time,
        collision_time_other,
        intersection_point,
        hit_succeed,
        CrossProduct(b,d).z,
        CrossProduct(d,b).z
    };

}

RayVsCircleCollisionData RayVsCircle(Ray2 my_ray, Circle* circle){
    bool hit = false;

    float ax = my_ray.Start().x;
    float ay = my_ray.Start().y;
    float bx = my_ray.End().x;
    float by = my_ray.End().y;

    float c = std::pow(ax-circle->position.x,2.0f) + std::pow(ay-circle->position.y,2.0f) - std::pow(circle->radius,2.0f);
    float b = 2.0f * ((ax-circle->position.x) * (bx - ax) + (ay-circle->position.y) * (by - ay));
    float a = std::pow(bx-ax,2.0f)+std::pow(by-ay,2.0f);

    //Console::Out(circle->position);
    //Console::Out(a,b,c);

    float x1 = ufoMaths::PositiveReducedQuadraticEquation(a,b,c);
    float x2 = ufoMaths::NegativeReducedQuadraticEquation(a,b,c);

    //If things don't add up
    if(std::isinf(x1) || std::isinf(x2) || std::isnan(x1) || std::isnan(x2)){
        //hit = false;
    }

    float intersection_time = std::min(x1, x2);
    float other_intersection_time = std::max(x1, x2);

    if(x1 >= 0.0f && x1 <= 1.0f || x2 >= 0.0f && x2 <= 1.0f){
        
        hit = true;
    }

    olc::vf2d intersection_point = my_ray.Start() + intersection_time * my_ray.Size();
    olc::vf2d other_intersection_point = my_ray.Start() + other_intersection_time * my_ray.Size();

    olc::vf2d collision_normal = (intersection_point - circle->position).norm();

    //RayVsCircle should have its own collisiondata class
    RayVsCircleCollisionData collision_data = RayVsCircleCollisionData{
        intersection_time,
        other_intersection_time,
        intersection_point,
        other_intersection_point,
        collision_normal,
        hit
    };

    return collision_data;
}

float SignFloat(float _number){
    if(std::abs(_number) < 0.001f) return 0.0f;
    else if(_number > 0.0f) return 1.0f;
    return -1.0f;
}

int SignInt(int _number){
    if(_number < 0) return -1;
    if(_number > 0) return 1;
    return 0;
}

float PreciseSignFloat(float _number){
    if(_number < 0.0f) return -1.0f;
    if(_number > 0.0f) return 1.0f;
    return 0.0f;
}

/*CollisionData RayVsRay(Ray2 _v, Ray2 _w){
    //Equation

    //_v.p0.x + _v.size().x * s = _w.p0.x + _w.size().x * t
    //_v.p0.y + _v.size().y * s = _w.p0.y + _w.size().y * t

    //solve for t
    //t = (_v.p0.x + _v.Size().x * s - _w.p0.x) / _w.Size().x    

    //solve for s

    //_v.y + _v.Size().y * s = _w.y + _w.Size().y * ((_v.x + _v.Size().x * s - _w.x) / _w.Size().x)

    //_v.y + _v.Size().y * s = _w.y + (_w.Size().y / _w.Size().x) * (_v.x + _v.Size().x * s - _w.x)        

    //(_w.Size().y / _w.Size().x) = [w.(h/w)]

    //_v.y + _v.Size().y * s = _w.y + [w.(h/w)] * (_v.x + _v.Size().x * s - _w.x)

    //_v.y + _v.Size().y * s = _w.y + [w.(h/w)] * _v.x + [w.(h/w)] * _v.Size().x * s - [w.(h/w)] * _w.x

    //_v.y + _v.Size().y * s - [w.(h/w)] * _v.Size().x * s = _w.y + [w.(h/w)] * _v.x - [w.(h/w)] * _w.x

    //_v.y + (_v.Size().y - [w.(h/w)] * _v.Size().x) * s = _w.y + [w.(h/w)] * _v.x - [w.(h/w)] * _w.x

    //(_v.Size().y - [w.(h/w)] * _v.Size().x) * s = _w.y + [w.(h/w)] * _v.x - [w.(h/w)] * _w.x - _v.y

    // s = (_w.y + [w.(h/w)] * _v.x - [w.(h/w)] * _w.x - _v.y) / (_v.Size().y - [w.(h/w)] * _v.Size().x)

    //"width delta-size"
    float w_ds = (_w.Size().y / _w.Size().x);
    //Console::Out(w_ds);

    float collision_time = (_w.p0.y + w_ds * _v.p0.x - w_ds * _w.p0.x - _v.p0.y) / (_v.Size().y - w_ds * _v.Size().x);

    float v_ds = (_v.Size().y / _v.Size().x);
    Console::Out(v_ds, _v.Size().y, _v.Size().x);

    float collision_time_other = (_v.p0.y + v_ds * _w.p0.x - v_ds * _v.p0.x - _w.p0.y) / (_w.Size().y - v_ds * _w.Size().x);

    //------------------------------------------------------------------------------------------

    //_v.p0.y + _v.size().y * s = _w.p0.y + (_w.size().y / _w.size().x) * (_v.p0.x + _v.size().x * s - _w.p0.x)

    //_v.p0.y + _v.size().y * s = _w.p0.y + (_w.size().y / _w.size().x) * _v.p0.x + (_w.size().y / _w.size().x) * _v.size().x * s - (_w.size().y / _w.size().x) * _w.p0.x

    //_v.p0.y + _v.size().y * s - (_w.size().y / _w.size().x) * _v.size().x * s = _w.p0.y + (_w.size().y / _w.size().x) * _v.p0.x - (_w.size().y / _w.size().x) * _w.p0.x

    //(_v.p0.y + _v.size().y - (_w.size().y / _w.size().x) * _v.size().x) * s = _w.p0.y + (_w.size().y / _w.size().x) * _v.p0.x - (_w.size().y / _w.size().x) * _w.p0.x

    //float collision_time = (_w.p0.y + (_w.Size().y / _w.Size().x) * _v.p0.x - (_w.Size().y / _w.Size().x) * _w.p0.x) / (_v.p0.y + _v.Size().y - (_w.Size().y / _w.Size().x) * _v.Size().x);
    
    //float collision_time_other = (_v.p0.y + (_v.Size().y / _v.Size().x) * _w.p0.x - (_v.Size().y / _v.Size().x) * _v.p0.x) / (_w.p0.y + _w.Size().y - (_v.Size().y / _v.Size().x) * _w.Size().x);
    
    olc::vf2d intersection_point = _v.Start() + _v.Size() * collision_time;
    
    bool hit_succeed = false;
    
    if((0.0f <= collision_time && collision_time <= 1.0f) && (0.0f <= collision_time_other && collision_time_other <= 1.0f)){
        hit_succeed = true;
    }

    return CollisionData{
        collision_time,
        collision_time_other,
        intersection_point,
        hit_succeed
    };
}*/    

/*
float RayVsPlane(Ray3 _ray, Triangle3 _triangle){
    Vector3 n = CrossProduct(_triangle.p1, _triangle.p2, _triangle.p3);

    float a = n.x * (_ray.p2.x - _ray.p1.x) + n.y * (_ray.p2.y - _ray.p1.y) + n.z * (_ray.p2.z - _ray.p1.z);
    float b = _ray.p1.x * n.x + _ray.p1.y * n.y + _ray.p1.z * n.z;
    float c = _triangle.p1.x * n.x + _triangle.p1.y * n.y + _triangle.p1.z * n.z;

    float t = -(b-c)/a;
}
*/

float Distance2(olc::vf2d _p0, olc::vf2d _p1){
    return std::sqrt((_p0-_p1).x*(_p0-_p1).x+(_p0-_p1).y*(_p0-_p1).y);
}

bool
RectangleVsRectangle(const ufo::Rectangle& _rect_1, const ufo::Rectangle& _rect_2){
    if (
        (_rect_1.position.x > (_rect_2.position.x + _rect_2.size.x)) || // is the left side greater than the other rect's right side?
        ((_rect_1.position.x + _rect_1.size.x) < _rect_2.position.x) ||       // is the right side less than the other rect's left side?
        (_rect_1.position.y > (_rect_2.position.y + _rect_2.size.y)) || // is the lower side greater than the other rect's upper side?
        ((_rect_1.position.y + _rect_1.size.y) < _rect_2.position.y))         // is the upper side less than the other rect's lower side?
    {
        return false;
    }
    return true;
}

//Does rect 1 contain rect 2?
bool
RectangleContainsRectangle(const ufo::Rectangle& _rect_1, const ufo::Rectangle& _rect_2){
    if(
        _rect_2.position.x >= _rect_1.position.x && _rect_2.position.x <= _rect_1.position.x + _rect_1.size.x &&
        _rect_2.position.x + _rect_2.size.x >= _rect_1.position.x && _rect_2.position.x + _rect_2.size.x <= _rect_1.position.x + _rect_1.size.x &&
        _rect_2.position.y >= _rect_1.position.y && _rect_2.position.y <= _rect_1.position.y + _rect_1.size.y &&
        _rect_2.position.y + _rect_2.size.y >= _rect_1.position.y && _rect_2.position.y + _rect_2.size.y <= _rect_1.position.y + _rect_1.size.y
    ){
        return true;
    }
    return false;
}

bool
IsOverlapping(const ufo::Rectangle& _rect_1, const ufo::Rectangle& _rect_2){
    return RectangleVsRectangle(_rect_1, _rect_2);
}

bool
CircleVsCircle(const Circle& _circle_1, const Circle& _circle_2){
    return (Distance2(_circle_1.position, _circle_2.position) <= _circle_1.radius+_circle_2.radius);
}

bool
IsOverlapping(const Circle& _circle_1, const Circle& _circle_2){
    return CircleVsCircle(_circle_1, _circle_2);
}

bool CircleVsPoint(const Circle& _circle, olc::vf2d _point){
    return (Distance2(_circle.position, _point) <= _circle.radius);
}

/*bool
IsOverlapping(const Circle& _circle, olc::vf2d _point){
    return CircleVsPoint(_circle, _point);
}

bool
CircleVsRay(const Circle& _circle, const Ray2& _ray){
    return(
        CircleVsPoint(_circle, _ray.Start()) ||
        CircleVsPoint(_circle, _ray.End())
    );
}*/

/*bool
IsOverlapping(const Circle& _circle, const Ray2& _ray){
    return CircleVsRay(_circle, _ray);
}*/

bool
CircleVsRectangle(const Circle& _circle, const ufo::Rectangle& _rectangle){
    if(
        Distance2(_circle.position, _rectangle.position) <= _circle.radius ||
        Distance2(_circle.position, _rectangle.position+olc::vf2d(0.0f, _rectangle.size.y)) <= _circle.radius ||
        Distance2(_circle.position, _rectangle.position+olc::vf2d(_rectangle.size.x,0.0f)) <= _circle.radius ||
        Distance2(_circle.position, _rectangle.position + _rectangle.size) <= _circle.radius
    ){
        return true;
    }

    if(
        RectangleVsPoint(_rectangle, _circle.position+olc::vf2d(_circle.radius,0.0f)) ||
        RectangleVsPoint(_rectangle, _circle.position+olc::vf2d(-_circle.radius,0.0f)) ||
        RectangleVsPoint(_rectangle, _circle.position+olc::vf2d(0.0f, _circle.radius)) ||
        RectangleVsPoint(_rectangle, _circle.position+olc::vf2d(0.0f, -_circle.radius))
    ){
        return true;
    }
    
    return false;
}

bool RectangleVsCircle(const ufo::Rectangle& _rectangle, const Circle& _circle){
    return CircleVsRectangle(_circle, _rectangle);
}

//Not implemented yet
bool
IsOverlapping(const Circle& _circle, const ufo::Rectangle& _rectangle){
    return CircleVsRectangle(_circle, _rectangle);
}

//Not implemented yet
bool
IsOverlapping(const ufo::Rectangle& _rectangle, const Circle& _circle){
    return CircleVsRectangle(_circle, _rectangle);
}

bool
RectangleVsPoint(const ufo::Rectangle& _rect, olc::vf2d _point){
    if (_point.x < _rect.position.x ||
        _point.x > _rect.position.x + _rect.size.x ||
        _point.y < _rect.position.y ||
        _point.y > _rect.position.y + _rect.size.y
        )         // is the upper side less than the other rect's lower side?
    {
        return false;
    }
    return true;
}

// Pill

bool PillVsRectangle(const Pill& _pill, const ufo::Rectangle& _rectangle){
    return false;
}

bool PillVsCircle(const Pill& _pill, const Circle& _circle){
    Circle top_circle = Circle(_pill.position, _pill.radius);
    Circle bottom_circle = Circle(_pill.position+olc::vf2d(0.0f, _pill.height), _pill.radius);
    ufo::Rectangle middle_rectangle = ufo::Rectangle(_pill.position-olc::vf2d(_pill.radius ,0.0f), olc::vf2d(_pill.radius * 2.0f, _pill.height));
    if(CircleVsCircle(top_circle, _circle) || CircleVsCircle(bottom_circle, _circle) || RectangleVsCircle(middle_rectangle, _circle)) return true;

    return false;
}

bool CircleVsPill(const Circle& _circle, const Pill& _pill){
    return PillVsCircle(_pill, _circle);
}

bool IsOverlapping(const Pill& _pill, const ufo::Rectangle& _rectangle){
    return PillVsRectangle(_pill, _rectangle);
}

bool IsOverlapping(const Pill& _pill, const Circle& _circle){
    return PillVsCircle(_pill, _circle);
}

bool IsOverlapping(const Circle& _circle, const Pill& _pill){
    return PillVsCircle(_pill, _circle);
}

int Wrapi(int _value, int _min, int _max){
    assert(_min < _max);
    if(_value < 0) _value = std::abs(_max+_value%(_max-_min));
    
    int range = _max - _min; //1
    
    return _min + _value % range;
    
}

float DotProduct(olc::vf2d _v0, olc::vf2d _v1){
    return _v0.x * _v1.x + _v0.y * _v1.y;
}

//Work in progress. Do not use.
float PreciseSin(float _radians){
    _radians = Wrap(_radians, -PI, PI);
    return _radians-_radians*_radians*_radians*0.1666f+
        _radians*_radians*_radians*_radians*_radians*0.00833f-
        _radians*_radians*_radians*_radians*_radians*_radians*_radians*0.00019f; //+
        //_radians*_radians*_radians*_radians*_radians*_radians*_radians*_radians*_radians*0.000003f;
}


//Work in progress. Do not use.
float WIPSin(float _radians){
    _radians = Wrap(_radians, -PI, PI);

    float ret = 0.0f;

    float x_pow_2 = _radians * _radians;
    float x = x_pow_2;

    ret += _radians;
    ret -= x*_radians/6.0f;
    x*= x_pow_2;
    ret += x*_radians/120.0f;
    x*= x_pow_2;
    ret -= x*_radians/5040.0f;
    x*=x_pow_2;
    ret += x*_radians/362880.0f;

    //float x_pow_4 = x_pow_2*x_pow_2;
    //float x_pow_6 

    return ret;
}

//Work in progress. Do not use.
float WIPCos(float _radians){
    _radians = Wrap(_radians-PI*0.5f, -PI*0.5f, PI*1.5f);
    
    if(_radians < -PI*0.5f) _radians+=2.0f*(std::abs(_radians)-PI*0.5f);
    if(_radians > PI*0.5f) _radians-=2.0f*(_radians-PI*0.5f);

    return _radians-_radians*_radians*_radians*0.1666f+
        _radians*_radians*_radians*_radians*_radians*0.00833f-
        _radians*_radians*_radians*_radians*_radians*_radians*_radians*0.00019f; //+
        //_radians*_radians*_radians*_radians*_radians*_radians*_radians*_radians*_radians*0.000003f;
}

float PositiveReducedQuadraticEquation(float _a, float _b, float _c){
    float b = _b/_a;
    float c = _c/_a;

    float x = -b/2.0f + std::sqrt((b/2.0f)*(b/2.0f)-c);
    return x;
}

float NegativeReducedQuadraticEquation(float _a, float _b, float _c){
    float b = _b/_a;
    float c = _c/_a;

    float x = -b/2.0f - std::sqrt((b/2.0f)*(b/2.0f)-c);
    return x;
}

}