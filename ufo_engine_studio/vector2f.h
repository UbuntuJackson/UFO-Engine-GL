#pragma once
namespace UFOEngineStudio{
class Vector2f{
public:
    float x;
    float y;
    Vector2f(float _x, float _y) : x{_x}, y{_y}{}

    Vector2f operator+(const Vector2f& _other){
        return Vector2f(x+_other.x,y+_other.y);
    }

    Vector2f operator-(const Vector2f& _other){
        return Vector2f(x+_other.x,y+_other.y);
    }
};
}