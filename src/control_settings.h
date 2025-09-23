#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <SDL3/SDL.h>

namespace ufo{

class Controls{
public:
    std::unordered_map<std::string, unsigned int> controls;

    unsigned int GetControl(const std::string& _control_name){
        return controls.at(_control_name);
    }

};

class ControlSettings /*: public Widget*/{
public:

    std::vector<Controls> controls;

    ControlSettings(){
        controls.push_back(Controls());
        controls.back().controls["right"] = SDLK_RIGHT;
        controls.back().controls["left"] = SDLK_LEFT;
        controls.back().controls["up"] = SDLK_UP;
        controls.back().controls["down"] = SDLK_DOWN;
        controls.back().controls["shoot"] = SDLK_SPACE;
        controls.push_back(Controls());
        controls.back().controls["right"] = SDLK_D;
        controls.back().controls["left"] = SDLK_A;
        controls.back().controls["up"] = SDLK_W;
        controls.back().controls["down"] = SDLK_S;
        controls.back().controls["shoot"] = SDLK_LSHIFT;
        controls.push_back(Controls());
    }

    void SetupControls(Engine* _engine);

};

}