#include <memory>
#include <vector>
#include <string>
#include "../ufo_maths/ufo_maths.h"
#include "actor.h"
#include "actor_undo_and_redo.h"
#include "level.h"

namespace ufo{

ActorChange_AddActor::ActorChange_AddActor(Actor* _actor):actor{_actor}{}

void ActorChange_AddActor::Undo(){
    actor->stash = true;
}

void ActorChange_AddActor::Redo(){
    Console::PrintLine("Redo ActorChange_AddActor",actor->class_name,actor,"in",actor->parent->class_name);

    for(int a = 0; a < (int)actor->level->stashed_actors.size(); a++){
        if(actor->level->stashed_actors[a].get() == actor){
            actor->parent->AddActorUniquePtr(std::move(actor->level->stashed_actors[a]));
            actor->level->stashed_actors.erase(actor->level->stashed_actors.begin()+a);
            actor->stash = false;
            break;
        }
    }
}

ActorChange_RemoveActor::ActorChange_RemoveActor(Actor* _actor):actor{_actor}{}

void ActorChange_RemoveActor::Undo(){

    actor->parent->should_be_sorted = true;

    for(int a = 0; a < (int)actor->level->stashed_actors.size(); a++){
        if(actor->level->stashed_actors[a].get() == actor){
            Console::PrintLine("Undo ActorChange_RemoveActor",actor->class_name,actor,"in",actor->parent->class_name);
            actor->parent->AddActorUniquePtr(std::move(actor->level->stashed_actors[a]));
            actor->level->stashed_actors.erase(actor->level->stashed_actors.begin()+a);
            actor->stash = false;
            break;
        }
    }

}

void ActorChange_RemoveActor::Redo(){
    actor->stash = true;
}

}
