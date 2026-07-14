#pragma once
#include <memory>
#include <vector>
#include <string>
#include "../ufo_maths/ufo_maths.h"
#include "../ufo_garbage_collector/gc_json.h"
#include "../utils/conversion.h"

namespace UFOEngineStudio{class LevelEditorTab;}

namespace ufo{

class Actor;

class ActorChange{
public:
    virtual void Undo() = 0;
    virtual void Redo() = 0;
    virtual void Do() = 0;
    virtual ~ActorChange() = default;
    virtual std::string GetInfo() = 0;
};

class ActorChange_Move : public ActorChange{
private:
    UFOEngineStudio::LevelEditorTab* level_editor_tab = nullptr;
    int actor_id;
    int former_parent_id;
    int former_order_index;
    int current_parent_id;
    int current_order_index;
public:

    ActorChange_Move(
        UFOEngineStudio::LevelEditorTab* _level_editor_tab,
        int _actor, int _former_parent,
        int _former_order_index,
        int _current_parent,
        int _current_order_index);

    void Undo() override;

    void Redo() override;

    void Do() override;

    std::string GetInfo(){
        return "ActorChange_Move\n"
            "    former_order_index: "+std::to_string(former_order_index)+"\n    current_order_index: "+std::to_string(current_order_index);
    }
};

class ActorChange_AddActor : public ActorChange{
public:
    UFOEngineStudio::LevelEditorTab* level_editor_tab = nullptr;
    int actor_id;
    int parent_id;
    ActorChange_AddActor(UFOEngineStudio::LevelEditorTab* _level_editor_tab ,int _actor_id, int _parent_id);

    void Undo() override;

    void Redo() override;

    void Do() override{}

    std::string GetInfo(){
        return "ActorChange_Move "+std::to_string(actor_id);
    }

};

class ActorChange_RemoveActor : public ActorChange{
public:
    UFOEngineStudio::LevelEditorTab* level_editor_tab = nullptr;
    int actor_id;
    int parent_id;

    ActorChange_RemoveActor(UFOEngineStudio::LevelEditorTab* _level_editor_tab ,int _actor_id, int _parent_id);

    void Undo() override;

    void Redo() override;

    void Do() override{}

    std::string GetInfo(){
        return "ActorChange_Move "+std::to_string(actor_id);
    }

};

class ActorChange_RemoveMultipleActors : public ActorChange{
public:
    std::vector<ActorChange_RemoveActor> changes;

    ActorChange_RemoveMultipleActors() = default;

    void Undo() override;
    void Redo() override;
    void Do() override;

    std::string GetInfo(){

        std::string info = "";
        for(ActorChange_RemoveActor change : changes){
            info+=change.GetInfo()+"\n";
        }

        return info;
    }

};

class ActorChange_CustomVariableInt : public ActorChange{
private:
    UFOEngineStudio::LevelEditorTab* level_editor_tab = nullptr;
    int actor_id = ufo::Maths::NULL_ID;

    std::string variable_name;
    int former_value = 0;
    int current_value = 0;

public:
    ActorChange_CustomVariableInt(UFOEngineStudio::LevelEditorTab* _level_editor_tab ,int _actor_id, std::string _variable_name, int _former_value, int _current_value);

    void Undo() override;
    void Redo() override;
    void Do() override;

    std::string GetInfo(){
        return "ActorChange_CustomVariableInt";
    }
};

class ActorChange_CustomVariableFloat : public ufo::ActorChange{
private:

    UFOEngineStudio::LevelEditorTab* level_editor_tab = nullptr;
    int actor_id = ufo::Maths::NULL_ID;

    std::string variable_name;
    float former_value = 0;
    float current_value = 0;

public:
    ActorChange_CustomVariableFloat(UFOEngineStudio::LevelEditorTab* _level_editor_tab ,int _actor_id, std::string _variable_name, float _former_value, float _current_value);

    void Undo() override;
    void Redo() override;
    void Do() override;

    std::string GetInfo(){
        return "ActorChange_CustomVariableFloat";
    }
};

class ActorChange_CustomVariableFloatHandle : public ActorChange{
private:

    UFOEngineStudio::LevelEditorTab* level_editor_tab = nullptr;
    int actor_id = ufo::Maths::NULL_ID;
    std::string name;

    float former_value = 0;
    float current_value = 0;

public:
    ActorChange_CustomVariableFloatHandle(
        UFOEngineStudio::LevelEditorTab* _level_editor_tab,
        int _actor_id,
        const std::string& _name,
        float _former_value,
        float _current_value);

    void Undo() override;
    void Redo() override;
    void Do() override;

    std::string GetInfo(){
        return "ActorChange_CustomVariableFloatHandle";
    }
};

class ActorChange_CustomVariableIntHandle : public ActorChange{
private:

    UFOEngineStudio::LevelEditorTab* level_editor_tab = nullptr;
    int actor_id = ufo::Maths::NULL_ID;
    std::string name;

    int former_value = 0;
    int current_value = 0;

public:
    ActorChange_CustomVariableIntHandle(
        UFOEngineStudio::LevelEditorTab* _level_editor_tab,
        int _actor_id,
        const std::string& _name,
        int _former_value,
        int _current_value);

    void Undo() override;
    void Redo() override;
    void Do() override;

    std::string GetInfo(){
        return "ActorChange_CustomVariableIntHandle";
    }
};

// ...

class ActorChange_CustomVariableStringHandle : public ActorChange{
private:

    UFOEngineStudio::LevelEditorTab* level_editor_tab = nullptr;
    int actor_id = ufo::Maths::NULL_ID;
    std::string name;

    std::string former_value = 0;
    std::string current_value = 0;

public:
    ActorChange_CustomVariableStringHandle(
        UFOEngineStudio::LevelEditorTab* _level_editor_tab,
        int _actor_id,
        const std::string& _name,
        const std::string& _former_value,
        const std::string& _current_value);

    void Undo() override;
    void Redo() override;
    void Do() override;

    std::string GetInfo(){
        return "ActorChange_CustomVariableStringHandle";
    }
};

// ...

class ActorChange_CustomVariableVector2fHandle : public ActorChange{
private:

    UFOEngineStudio::LevelEditorTab* level_editor_tab = nullptr;
    int actor_id = ufo::Maths::NULL_ID;
    std::string name;

    Vector2f former_value;
public:
    Vector2f current_value;

public:
    ActorChange_CustomVariableVector2fHandle(UFOEngineStudio::LevelEditorTab* _level_editor_tab,
    int _actor_id,
    const std::string& _name,
    Vector2f _former_value,
    Vector2f _current_value);

    void Undo() override;
    void Redo() override;
    void Do() override;

    std::string GetInfo(){
        return "ActorChange_CustomVariableVector2fHandle";
    }
};

class ActorChange_MultipleActorChange : public ActorChange{
public:
    std::vector<std::unique_ptr<ActorChange>> changes;

    bool undo_in_reverse_specifically_for_redoing_actor_move_in_actor_tree = false;

public:
    ActorChange_MultipleActorChange(bool _undo_in_reverse_specifically_for_redoing_actor_move_in_actor_tree);

    void Undo() override;
    void Redo() override;
    void Do() override;

    std::string GetInfo(){
        std::string info;

        for(const auto& change : changes){
            info+="    "+change->GetInfo()+"\n";
        }

        return "ActorChange_MultipleActorChange:\n"+ info;
    }
};

}
