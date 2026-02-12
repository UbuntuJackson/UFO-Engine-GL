#pragma once
#include <memory>
#include <vector>
#include <string>
#include "../ufo_maths/ufo_maths.h"

class Actor;

namespace ufo{

class ActorChange{
public:
    virtual void Undo() = 0;
    virtual void Redo() = 0;
    virtual void Do() = 0;
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

}
