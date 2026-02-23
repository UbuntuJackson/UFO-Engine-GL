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

class ActorChange_RemoveMultipleActors : public ActorChange{
public:
    std::vector<ActorChange_RemoveActor> changes;

    ActorChange_RemoveMultipleActors() = default;

    void Undo() override;

    void Redo() override;

    void Do() override;

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

class ActorChange_CustomVariableFloatHandle : public ufo::ActorChange{
private:

    float* ptr;

    float former_value = 0;
    float current_value = 0;

public:
    ActorChange_CustomVariableFloatHandle(float* _ptr, float _former_value, float _current_value);

    void Undo() override;
    void Redo() override;
    void Do() override;
};

class ActorChange_CustomVariableIntHandle : public ufo::ActorChange{
private:

    int* ptr;

    int former_value = 0;
    int current_value = 0;

public:
    ActorChange_CustomVariableIntHandle(int* _ptr, int _former_value, int _current_value);

    void Undo() override;
    void Redo() override;
    void Do() override;
};

// ...

class ActorChange_CustomVariableStringHandle : public ufo::ActorChange{
private:

    std::string* ptr;

    std::string former_value = 0;
    std::string current_value = 0;

public:
    ActorChange_CustomVariableStringHandle(std::string* _ptr, const std::string& _former_value, const std::string& _current_value);

    void Undo() override;
    void Redo() override;
    void Do() override;
};

// ...

class ActorChange_CustomVariableVector2fHandle : public ufo::ActorChange{
private:

    Vector2f* ptr;

    Vector2f former_value;
public:
    Vector2f current_value;

public:
    ActorChange_CustomVariableVector2fHandle(Vector2f* _ptr, Vector2f _former_value, Vector2f _current_value);

    void Undo() override;
    void Redo() override;
    void Do() override;
};

class ActorChange_MultipleActorChange : public ufo::ActorChange{
public:
    std::vector<std::unique_ptr<ActorChange>> changes;

public:
    ActorChange_MultipleActorChange();

    void Undo() override;
    void Redo() override;
    void Do() override;
};

}
