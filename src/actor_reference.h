#pragma once

#include "actor.h"

//If instances of this can just be checked every frame, then that would pretty much guarantee that actor is checked if alive every frame

template <class tActor>
class ActorReference{
private:
    tActor* actor = nullptr;
public:
    ActorReference(tActor* _actor) : actor{_actor}{}
    
    A* operator->(){
        if(actor->is_dead) actor = nullptr;
        return actor;
    }
};