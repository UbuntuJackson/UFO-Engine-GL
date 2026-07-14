#include <memory>
#include <vector>
#include <string>
#include "../ufo_maths/ufo_maths.h"
#include "actor.h"
#include "actor_undo_and_redo.h"
#include "editor_property.h"
#include "level.h"
#include "level_editor_tab.h"

namespace ufo{

ActorChange_Move::ActorChange_Move(UFOEngineStudio::LevelEditorTab* _level_editor_tab,int _actor_id, int _former_parent_id, int _former_order_index, int _current_parent_id, int _current_order_index) :
level_editor_tab{_level_editor_tab},
actor_id{_actor_id},
former_parent_id{_former_parent_id},
former_order_index{_former_order_index},
current_parent_id{_current_parent_id},
current_order_index{_current_order_index}
{

}

void ActorChange_Move::Undo(){
    ufo::Actor* actor = level_editor_tab->this_level->actors_with_stable_id.at(actor_id);
    ufo::Actor* current_parent = level_editor_tab->this_level->actors_with_stable_id.at(current_parent_id);
    ufo::Actor* former_parent = level_editor_tab->this_level->actors_with_stable_id.at(former_parent_id);

    current_order_index = actor->order_index;

    former_parent->InsertActorUniquePtr(std::move(current_parent->actors[actor->order_index]), former_order_index);

    current_parent->actors.erase(current_parent->actors.begin()+actor->order_index);

}

void ActorChange_Move::Redo(){
    ufo::Actor* actor = level_editor_tab->this_level->actors_with_stable_id.at(actor_id);
    ufo::Actor* current_parent = level_editor_tab->this_level->actors_with_stable_id.at(current_parent_id);
    ufo::Actor* former_parent = level_editor_tab->this_level->actors_with_stable_id.at(former_parent_id);

    current_parent->InsertActorUniquePtr(std::move(former_parent->actors[former_order_index]), current_order_index);

    former_order_index = actor->order_index;

    former_parent->actors.erase(former_parent->actors.begin()+former_order_index);

    current_parent->should_be_sorted = true;
    former_parent->should_be_sorted = true;

    Console::PrintLine("ActorChange_Move Undo",actor->editor_name, former_parent->editor_name, former_order_index);
}

void ActorChange_Move::Do(){

}

//ActorChange_AddActor

ActorChange_AddActor::ActorChange_AddActor(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _actor_id, int _parent_id):
    level_editor_tab{_level_editor_tab}, actor_id{_actor_id}, parent_id{_parent_id}{}

void ActorChange_AddActor::Undo(){
    ufo::Actor* actor = level_editor_tab->this_level->actors_with_stable_id.at(actor_id);
    actor->stash = true;
}

void ActorChange_AddActor::Redo(){
    ufo::Actor* actor = level_editor_tab->this_level->actors_with_stable_id.at(actor_id);
    ufo::Actor* parent = level_editor_tab->this_level->actors_with_stable_id.at(parent_id);

    for(int a = 0; a < (int)actor->level->stashed_actors.size(); a++){
        if(actor->level->stashed_actors[a].get() == actor){
            parent->AddActorUniquePtr(std::move(actor->level->stashed_actors[a]));

            //actor->parent->InsertActorUniquePtr(std::move(actor->level->stashed_actors[a]), actor->order_index);

            actor->level->stashed_actors.erase(actor->level->stashed_actors.begin()+a);
            actor->stash = false;
            break;
        }
    }
}

ActorChange_RemoveActor::ActorChange_RemoveActor(UFOEngineStudio::LevelEditorTab* _level_editor_tab ,int _actor_id,int _parent_id):
    level_editor_tab{_level_editor_tab},
    actor_id{_actor_id},
    parent_id{_parent_id}
    {}

void ActorChange_RemoveActor::Undo(){
    ufo::Actor* actor = level_editor_tab->this_level->actors_with_stable_id.at(actor_id);
    ufo::Actor* parent = level_editor_tab->this_level->actors_with_stable_id.at(parent_id);

    parent->should_be_sorted = true;

    for(int a = 0; a < (int)actor->level->stashed_actors.size(); a++){
        if(actor->level->stashed_actors[a].get() == actor){
            Console::PrintLine("Undo ActorChange_RemoveActor",actor->class_name,actor,"in",parent->class_name);
            //actor->parent->AddActorUniquePtr(std::move(actor->level->stashed_actors[a]));

            parent->InsertActorUniquePtr(std::move(actor->level->stashed_actors[a]), actor->order_index);

            actor->level->stashed_actors.erase(actor->level->stashed_actors.begin()+a);
            actor->stash = false;
            break;
        }
    }

}

void ActorChange_RemoveActor::Redo(){
    ufo::Actor* actor = level_editor_tab->this_level->actors_with_stable_id.at(actor_id);
    actor->stash = true;
}

//...

void ActorChange_RemoveMultipleActors::Redo(){
    for(ActorChange_RemoveActor change : changes){
        change.Redo();
    }
}

void ActorChange_RemoveMultipleActors::Undo(){

    for(ActorChange_RemoveActor change : changes){
        change.Undo();
    }

}

void ActorChange_RemoveMultipleActors::Do(){

}

//...

ActorChange_CustomVariableInt::ActorChange_CustomVariableInt(UFOEngineStudio::LevelEditorTab* _level_editor_tab ,int _actor_id,  std::string _variable_name, int _former_value, int _current_value) :
    level_editor_tab{_level_editor_tab},
    actor_id{_actor_id},
    variable_name{_variable_name},
    former_value{_former_value},
    current_value{_current_value}{}

void ActorChange_CustomVariableInt::Undo() {
    ufo::Actor* actor = level_editor_tab->this_level->actors_with_stable_id.at(actor_id);
    for(const auto& property : actor->editor_properties){
        ufo::EditorPropertyInt* converted_property = dynamic_cast<ufo::EditorPropertyInt*>(property.get());

        if(converted_property && converted_property->variable_name == variable_name){
            converted_property->value = former_value;
        }
    }
}
void ActorChange_CustomVariableInt::Redo() {
    ufo::Actor* actor = level_editor_tab->this_level->actors_with_stable_id.at(actor_id);
    for(const auto& property : actor->editor_properties){
        ufo::EditorPropertyInt* converted_property = dynamic_cast<ufo::EditorPropertyInt*>(property.get());

        if(converted_property && converted_property->variable_name == variable_name){
            converted_property->value = current_value;
        }
    }
}
void ActorChange_CustomVariableInt::Do() {}


ActorChange_CustomVariableFloat::ActorChange_CustomVariableFloat(
    UFOEngineStudio::LevelEditorTab* _level_editor_tab,
    int _actor_id,
    std::string _variable_name,
    float _former_value,
    float _current_value) :
    level_editor_tab{_level_editor_tab},
    actor_id{_actor_id},
    variable_name{_variable_name},
    former_value{_former_value},
    current_value{_current_value}{}

void ActorChange_CustomVariableFloat::Undo() {
    ufo::Actor* actor = level_editor_tab->this_level->actors_with_stable_id.at(actor_id);
    for(const auto& property : actor->editor_properties){
        ufo::EditorPropertyFloat* converted_property = dynamic_cast<ufo::EditorPropertyFloat*>(property.get());

        if(converted_property && converted_property->variable_name == variable_name){
            converted_property->value = former_value;
            break;
        }
    }

    Console::PrintLine("[UFO-Engine Studio] ActorChange_CustomVariableFloat::Undo: Could not find field", variable_name,"; Perhaps it was removed.");
}
void ActorChange_CustomVariableFloat::Redo() {
    ufo::Actor* actor = level_editor_tab->this_level->actors_with_stable_id.at(actor_id);
    for(const auto& property : actor->editor_properties){
        ufo::EditorPropertyFloat* converted_property = dynamic_cast<ufo::EditorPropertyFloat*>(property.get());

        if(converted_property && converted_property->variable_name == variable_name){
            converted_property->value = current_value;
        }
    }
}
void ActorChange_CustomVariableFloat::Do() {}

// ...

ActorChange_CustomVariableFloatHandle::ActorChange_CustomVariableFloatHandle(
    UFOEngineStudio::LevelEditorTab* _level_editor_tab,
    int _actor_id,
    const std::string& _name,
    float _former_value,
    float _current_value
) :
    level_editor_tab{_level_editor_tab},
    actor_id{_actor_id},
    name{_name},
    former_value{_former_value},
    current_value{_current_value}{}

void ActorChange_CustomVariableFloatHandle::Undo() {

    ufo::Actor* actor = level_editor_tab->this_level->actors_with_stable_id.at(actor_id);
    *actor->float_handles.at(name) = former_value;

}
void ActorChange_CustomVariableFloatHandle::Redo() {
    ufo::Actor* actor = level_editor_tab->this_level->actors_with_stable_id.at(actor_id);
    *actor->float_handles.at(name) = current_value;

}
void ActorChange_CustomVariableFloatHandle::Do() {}

// ...

ActorChange_CustomVariableIntHandle::ActorChange_CustomVariableIntHandle(UFOEngineStudio::LevelEditorTab* _level_editor_tab,
int _actor_id,
const std::string& _name,
int _former_value,
int _current_value) :
level_editor_tab{_level_editor_tab},
actor_id{_actor_id},
name{_name},
former_value{_former_value},
current_value{_current_value}{}

void ActorChange_CustomVariableIntHandle::Undo() {

    ufo::Actor* actor = level_editor_tab->this_level->actors_with_stable_id.at(actor_id);
    *actor->int_handles.at(name) = former_value;

}
void ActorChange_CustomVariableIntHandle::Redo() {
    ufo::Actor* actor = level_editor_tab->this_level->actors_with_stable_id.at(actor_id);
    *actor->int_handles.at(name) = current_value;

}
void ActorChange_CustomVariableIntHandle::Do() {}

// ...

ActorChange_CustomVariableStringHandle::ActorChange_CustomVariableStringHandle(
UFOEngineStudio::LevelEditorTab* _level_editor_tab,
int _actor_id,
const std::string& _name,
const std::string& _former_value,
const std::string& _current_value
) :
    level_editor_tab{_level_editor_tab},
    actor_id{_actor_id},
    name{_name},
    former_value{_former_value},
    current_value{_current_value}
    {}

void ActorChange_CustomVariableStringHandle::Undo() {

    ufo::Actor* actor = level_editor_tab->this_level->actors_with_stable_id.at(actor_id);
    *actor->string_handles.at(name) = former_value;

}
void ActorChange_CustomVariableStringHandle::Redo() {
    ufo::Actor* actor = level_editor_tab->this_level->actors_with_stable_id.at(actor_id);
    *actor->string_handles.at(name) = current_value;

}
void ActorChange_CustomVariableStringHandle::Do() {}

// ...

ActorChange_CustomVariableVector2fHandle::ActorChange_CustomVariableVector2fHandle(
UFOEngineStudio::LevelEditorTab* _level_editor_tab,
int _actor_id,
const std::string& _name,
Vector2f _former_value,
Vector2f _current_value) :
    level_editor_tab{_level_editor_tab},
    actor_id{_actor_id},
    name{_name},
    former_value{_former_value},
    current_value{_current_value}{}

void ActorChange_CustomVariableVector2fHandle::Undo() {

    ufo::Actor* actor = level_editor_tab->this_level->actors_with_stable_id.at(actor_id);
    *actor->vector2f_handles.at(name) = former_value;

}
void ActorChange_CustomVariableVector2fHandle::Redo() {
    ufo::Actor* actor = level_editor_tab->this_level->actors_with_stable_id.at(actor_id);
    *actor->vector2f_handles.at(name) = current_value;

}
void ActorChange_CustomVariableVector2fHandle::Do() {}

// ...

ActorChange_MultipleActorChange::ActorChange_MultipleActorChange(bool _undo_in_reverse_specifically_for_redoing_actor_move_in_actor_tree)
:undo_in_reverse_specifically_for_redoing_actor_move_in_actor_tree{_undo_in_reverse_specifically_for_redoing_actor_move_in_actor_tree}{}

void ActorChange_MultipleActorChange::Undo() {
    for(int a = (int)changes.size()-1; a != -1; a--) changes[a]->Undo();

}

void ActorChange_MultipleActorChange::Redo() {
    if(!undo_in_reverse_specifically_for_redoing_actor_move_in_actor_tree) for(int a = 0; a < (int)changes.size(); a++) changes[a]->Redo();
    else for(int a = (int)changes.size()-1; a != -1; a--) changes[a]->Redo();

}
void ActorChange_MultipleActorChange::Do() {
    //for(int a = 0; a < (int)changes.size(); a++) changes[a]->Do();
}

}
