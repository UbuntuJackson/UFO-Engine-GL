#pragma once
namespace ufo{

class Colour{
public:
    float r;
    float g;
    float b;
    float a;
    Colour(float _r, float _g, float _b, float _a) :
    r{_r},g{_g},b{_a} {}

    static Colour CreateFrom0To255Range(int _r, int _g, int _b, int _a){
        return Colour(float(_r)/255.0f, float(_g)/255.0f, float(_b)/255.0f, float(_a)/255.0f);
    }

    int GetRFrom255Range(){
        return int(r*255.0f);
    }
    int GetGFrom255Range(){
        return int(g*255.0f);
    }
    int GetBFrom255Range(){
        return int(b*255.0f);
    }
    int GetAFrom255Range(){
        return int(a*255.0f);
    }

};


}