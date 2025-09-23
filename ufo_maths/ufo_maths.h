#ifndef UFO_MATHS_H
#define UFO_MATHS_H
#include <cmath>
#include "../external/olcPixelGameEngine.h"
#include "../ufo_collision/collision_data.h"
#include "../shapes/ray2.h"
#include "../shapes/rectangle.h"
#include "../shapes/circle.h"
//#include "../shapes/shape.h"
#include <cassert>
typedef olc::vf2d Vector2f;
typedef olc::vi2d Vector2i;

namespace ufoMaths{

//#define SIN(a) std::sin(a)
//#define COS(a) std::cos(a)

CollisionData RayVsRay(Ray2 _v, Ray2 _w);

//RayVsCircleCollisionData RayVsCircle(Ray2 my_ray, Shape<Circle>* circle);
RayVsCircleCollisionData RayVsCircle(Ray2 my_ray, Circle* circle);

//Precision. Should there be a global threshold to round to zero? Like 0.001.

template<typename tNumber>
tNumber Sign(tNumber _number){
    if(std::abs(_number) < 0.001f) return 0.0f;
    else if(_number > 0.0f) return 1.0f;
    return -1.0f;
    
}

float SignFloat(float _number);

int SignInt(int _number);

float PreciseSignFloat(float _number);

template<typename tNumber>
bool AlmostZero(tNumber _number, tNumber _threshold){
    if(std::abs(_number) < std::abs(_threshold)) return true;
    return false;
}

template<typename tNumber>
tNumber Clamp(tNumber _number, tNumber _min, tNumber _max){
    if(_number < _min) return _min;
    if(_number > _max) return _max;
    return _number;
}

template<typename tSetOfNumbers>
tSetOfNumbers Lerp(tSetOfNumbers _p0, tSetOfNumbers _p1, float _scalar){
    return (_p1 - _p0) * _scalar;
}

float Distance2(olc::vf2d _p0, olc::vf2d _p1);

bool
RectangleVsRectangle(const ufo::Rectangle& _rect_1, const ufo::Rectangle& _rect_2);

bool
RectangleContainsRectangle(const ufo::Rectangle& _rect_1, const ufo::Rectangle& _rect_2);

bool
IsOverlapping(const ufo::Rectangle& _rect_1, const ufo::Rectangle& _rect_2);

bool
RectangleVsCircle(const ufo::Rectangle& _rectangle, const Circle& _circle);

bool
RectangleVsCircle(const Circle& _circle, const ufo::Rectangle& _rectangle);

bool
CircleVsRectangle(const Circle& _circle, const ufo::Rectangle& _rectangle);

bool
IsOverlapping(const Circle& _circle, const ufo::Rectangle& _rectangle);

bool
IsOverlapping(const ufo::Rectangle& _rectangle, const Circle& _circle);

bool
CircleVsCircle(const Circle& _circle_1, const Circle& _circle_2);

bool
IsOverlapping(const Circle& _circle_1, const Circle& _circle_2);

bool
CircleVsRay(const Circle& _circle, const Ray2& _ray);

bool
RectangleVsPoint(const ufo::Rectangle& _rect, olc::vf2d _point);

/*bool
IsOverlapping(const Rectangle& _rect, const olc::vf2d& _point);
*/
//bool
//CircleVsPoint(const Circle& _circle, const olc::vf2d& _point);

//bool
//IsOverlapping(const Circle& _circle, olc::vf2d _point);

//bool
//CircleVsRay(const Circle& _circle, const Ray2& _ray);

//bool
//IsOverlapping(const Circle& _circle, const Ray2& _ray);

/*
bool
IsOverlapping(const Circle& _rect, const olc::vf2d& _point);
*/

// Pill

bool PillVsRectangle(const Pill& _pill, const ufo::Rectangle& _rectangle);

bool PillVsCircle(const Pill& _pill, const Circle& _circle);
bool CircleVsPill(const Circle& _circle, const Pill& _pill);

bool IsOverlapping(const Pill& _pill, const ufo::Rectangle& _rectangle);

bool IsOverlapping(const Pill& _pill, const Circle& _circle);
bool IsOverlapping(const Circle& _circle, const Pill& _pill);

template<typename tNumber>
tNumber Wrap(tNumber _value, tNumber _min, tNumber _max){
    assert(_min < _max);
    tNumber value_relative_to_min = _value - _min;
    tNumber range = _max - _min;
    if(value_relative_to_min > 0){
        return _min + std::fmod(value_relative_to_min, range);
    }
    if(value_relative_to_min < 0){
        return _max + std::fmod(value_relative_to_min, range);
    }
    return _value;
}

int Wrapi(int _value, int _min, int _max);

float DotProduct(olc::vf2d _v0, olc::vf2d _v1);

const float PI = 3.141592653589793238464338328f;

float PreciseSin(float _radians);
float WIPSin(float _radians);

float WIPCos(float _radians);

/*constexpr float Sin(float _x){
    return std::sin(_x);
}

constexpr float Cos(float _x){
    return std::cos(_x);
}

constexpr float Tan(float _x){
    return std::cos(_x);
}

constexpr float Sqrt(float _x){
    return std::sqrt(_x);
}*/

float PositiveReducedQuadraticEquation(float _a, float _b, float _c);

float NegativeReducedQuadraticEquation(float _a, float _b, float _c);

}

#endif