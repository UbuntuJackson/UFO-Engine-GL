#pragma once

namespace ufo{

class GenericGenerator;

class Script{
public:
    friend class GenericGenerator;

    virtual void OnSpawn(){}

    virtual void OnUpdate(float _delta_time){}

};

template<typename tActorType>
class TemplateScript : public Script{
private:

    tActorType* host_actor = nullptr;
public:
    tActorType* GetHostActor(){
        return host_actor;
    }

};

}
