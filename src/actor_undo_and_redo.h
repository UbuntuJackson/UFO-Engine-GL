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
    virtual ~ActorChange() = default;
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

class ActorChange_EditorPropertyChange : public ActorChange, public ufo::gc::Root{
private:

    //Actor that this object points to should be alive all throughout this object's lifetime anyway.
    Actor* actor = nullptr;

    gc::JsonMap* property_snap_shot_as_json = nullptr;

public:
    ActorChange_EditorPropertyChange(Actor* _actor) :
        actor{_actor}
    {
        name = "GC_ActorChange_EditorPropertyChange";

        MakeMarkable(&property_snap_shot_as_json);

    }

    void Undo() override{}
    void Redo() override{}
    void Do() override{}

};

class ActorChange_CustomVariableInt : public ufo::ActorChange{
private:
    Actor* actor = nullptr;

    std::string variable_name;
    int former_value = 0;
    int current_value = 0;

public:
    ActorChange_CustomVariableInt(Actor* _actor, std::string _variable_name, int _former_value, int _current_value);

    void Undo() override;
    void Redo() override;
    void Do() override;
};

class ActorChange_CustomVariableFloat : public ufo::ActorChange{
private:
    Actor* actor = nullptr;

    std::string variable_name;
    float former_value = 0;
    float current_value = 0;

public:
    ActorChange_CustomVariableFloat(Actor* _actor, std::string _variable_name, float _former_value, float _current_value);

    void Undo() override;
    void Redo() override;
    void Do() override;
};

}
