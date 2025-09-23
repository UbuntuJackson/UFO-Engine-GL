#include "level.h"
#include "actor.h"
#include "../ufo_maths/ufo_maths.h"
#include "../utils/console.h"

Actor::Actor(Vector2f _local_position) : local_position{_local_position}{

}

Vector2f Actor::GetGlobalPosition(){
    if(parent == nullptr){
        return local_position;
    }
    return local_position + parent->GetGlobalPosition();
}

void Actor::MarkAllDead(){
    for(auto& actor : actors){
        actor->is_dead = true;
        Console::PrintLine("Deleted", actor.get());
    }
}

void Actor::CleanUpDeadActors(){

    for(int i = actors.size()-1; i != -1; i--){
        if(actors[i]->is_dead){
            actors[i]->MarkAllDead();
            Console::PrintLine("Deleted", actors[i].get());
            actors.erase(actors.begin()+i);
        }
        else{
            actors[i]->CleanUpDeadActors();
        }
    }

}

void Actor::OnSpawn(){

}

void Actor::OnAddActor(Actor* _actor){}

void Actor::Update(float _delta_time){
    for(const auto& actor : actors){
        actor->Update(_delta_time);
    }

    OnUpdate(_delta_time);
}