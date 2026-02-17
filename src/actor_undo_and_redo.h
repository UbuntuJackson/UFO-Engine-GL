#pragma once
#include <memory>
#include <vector>
#include <string>
#include "../ufo_maths/ufo_maths.h"
#include "../ufo_garbage_collector/gc_json.h"

class Actor;

namespace ufo{

class ActorChange{
public:
    virtual void Undo() = 0;
    virtual void Redo() = 0;
    virtual void Do() = 0;
};

class ActorChange_Move : public ActorChange{
private:
    Actor* actor;
    Actor* former_parent;
    int former_order_index;
    Actor* current_parent;
    int current_order_index;
public:

    ActorChange_Move(Actor* _actor, Actor* _former_parent, int _former_order_index, Actor* _current_parent, int _current_order_index);

    void Undo() override;

    void Redo() override;

    void Do() override;
};

class ActorChange_AddActor : public ActorChange{
public:
    Actor* actor = nullptr;
    ActorChange_AddActor(Actor* _actor);

    void Undo() override;

    void Redo() override;

    void Do() override{}

};

class ActorChange_RemoveActor : public ActorChange{
public:
    Actor* actor = nullptr;

    ActorChange_RemoveActor(Actor* _actor);

    void Undo() override;

    void Redo() override;

    void Do() override{}

};

/*class ActorChange_EditorPropertyChange : public ActorChange, public ufo::gc::Root{
public:
    Actor* actor = nullptr;

    gc::JsonMap* property_snap_shot_as_json = nullptr;

    ActorChange_EditorPropertyChange(Actor* _actor) : actor{_actor}{

        //Make

    }

    void Undo(){}
    void Redo(){}
    void Do(){}

    };*/

}
