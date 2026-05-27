#pragma once

namespace ufo{

class Actor;

class Script{
public:

    Actor* base = nullptr;

    virtual void OnSpawn(){}

    virtual void OnUpdate(float _delta_time){}

};

}
