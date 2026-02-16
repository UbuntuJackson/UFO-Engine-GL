#include <memory>
#include <vector>
#include <string>
#include "../ufo_maths/ufo_maths.h"
#include "actor.h"
#include "actor_undo_and_redo.h"
#include "level.h"

namespace ufo{

ActorChange_AddActor::ActorChange_AddActor(Actor* _actor):actor{_actor}{}

ActorChange_Move::ActorChange_Move(Actor* _actor, Actor* _former_parent, int _former_order_index, Actor* _current_parent, int _current_order_index) :
actor{_actor},
former_parent{_former_parent},
former_order_index{_former_order_index},
current_parent{_current_parent},
current_order_index{_current_order_index}
{
    Console::PrintLine("ActorChange_Move",_actor->editor_name,former_parent->editor_name,_former_order_index, current_parent->editor_name, _current_order_index);
}

void ActorChange_Move::Undo(){

    //former_parent->AddActorUniquePtr(std::move(actor->parent->actors[actor->order_index]));

    //assert(actor->order_index == current_order_index);
    assert(actor->parent == current_parent);

    current_order_index = actor->order_index;

    former_parent->InsertActorUniquePtr(std::move(current_parent->actors[actor->order_index]), former_order_index);

    current_parent->actors.erase(current_parent->actors.begin()+actor->order_index);

    //current_parent->should_be_sorted = true;
    //former_parent->should_be_sorted = true;

    Console::PrintLine("ActorChange_Move Undo",actor->editor_name, former_parent->editor_name, former_order_index);
}

void ActorChange_Move::Redo(){

    current_parent->InsertActorUniquePtr(std::move(former_parent->actors[former_order_index]), current_order_index);

    former_parent->actors.erase(former_parent->actors.begin()+former_order_index);

    current_parent->should_be_sorted = true;
    former_parent->should_be_sorted = true;

    Console::PrintLine("ActorChange_Move Undo",actor->editor_name, former_parent->editor_name, former_order_index);
}

void ActorChange_Move::Do(){

}

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
