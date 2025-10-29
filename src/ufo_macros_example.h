#pragma once
#include "ufo_macros.h"
#include "actor.h"
#include "graphics.h"

namespace game{

namespace name
{
 
    std::string m = "::";
    std::string n = "hello world";
    std::string o = "";
    std::string p = " ";
    std::string q = "class";
    
}

}

class Camera{

};

class ForwardDeclaredClass;

namespace test{

ufo_class()
class MyActor:public Actor{
public:

    MyActor() : Actor(Vector2f(0.0f, 0.0f)){}

    ufo::Colour colour = Colour(0,0,0,0);

    ufo_alias("Moving") ufo_bool()
    bool moving = false;

    ufo_alias("HP") ufo_int_range(0,100) ufo_comment(
        "This is just one of those good old comments so hello world I guess")
    int hp = 5;

    std::string s = "Test String";

    ufo_method() ufo_alias(":")
    void Method(){
        
    }

    void OnDraw(Camera* _camera){

        Actor::OnDraw(nullptr, _camera);
        
    }

};

}

namespace game{

class YourActor : Actor{
public:

    ufo_dropdown_menu("mode 1", "mode 2", "mode 3")
    int mode = 0;

    ufo_alias("HP");
    int hp = 100;

};

}

class OurActor{

};