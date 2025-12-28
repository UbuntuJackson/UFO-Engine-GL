#pragma once
#include "../object.h"
#include "../dynamic_array.h"

namespace beam::gc{

class Actor : public Object{
public:
    DynamicArray<Actor*> *actors;
    DynamicArray<Actor*> *new_actor_queue;
    Actor* parent = nullptr;

    Actor(){
        MakeMarkable(&actors);
        MakeMarkable(&parent);
        MakeMarkable(&new_actor_queue);
    }

    void AddActor(Actor* _actor){
        new_actor_queue->push_back(_actor);
    }

};

}
