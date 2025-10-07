#pragma once
#include <memory>
#include <vector>
#include "../ufo_maths/ufo_maths.h"
#include "graphics.h"
#include "../imgui/imgui.h"

namespace ufo{
    class Engine;
}

class Level;

//Using rule of 0 3 and 5 for all actors
class Actor{
public:

    Vector2f local_position;

    bool is_dead = false;

    Actor(Vector2f _local_position);
private:
    Actor* parent = nullptr;
public:
    ufo::Engine* engine = nullptr;
    Level* level = nullptr;

    Vector2f GetGlobalPosition();

    //Reserve a set number of actors?
    std::vector<std::unique_ptr<Actor>> actors;

    std::vector<std::unique_ptr<Actor>> new_actor_queue;

    void AddNewActors(){
        for(auto&& actor : new_actor_queue){
            actor->AddNewActors();
            actors.push_back(std::move(actor));
        }
        new_actor_queue.clear();
    }

    template <typename tActor, typename ...tArgs>
    tActor* AddActor(tArgs&& ..._args){
        auto u_actor = std::make_unique<tActor>(_args...);
        auto actor_ptr = u_actor.get();
        actor_ptr->parent = this;
        actor_ptr->level = level;
        OnAddActor(actor_ptr);
        actor_ptr->engine = engine;
        actor_ptr->OnSpawn();
        new_actor_queue.push_back(std::move(u_actor));
        return actor_ptr;
    }

    //Actor(const Actor&) = delete;
    //Actor(Actor&&) = delete;

    void CleanUpDeadActors();

    void MarkAllDead();

    virtual void OnSpawn();

    virtual void OnAddActor(Actor* _actor);

    virtual void Update(float _delta_time);

    virtual void OnUpdate(float _delta_time){

    }

    void WidgetDraw(ufo::Graphics* _graphics){
        OnWidgetDraw(_graphics);
        for(const auto& actor : actors){
            actor->WidgetDraw(_graphics);
        }
    }

    virtual void OnWidgetDraw(ufo::Graphics* _graphics){
        
    }

    void Draw(ufo::Graphics* _graphics, Camera* _camera){
        OnDraw(_graphics, _camera);
        for(const auto& actor : actors){
            actor->Draw(_graphics, _camera);
        }
    }

    virtual void OnDraw(ufo::Graphics* _graphics, Camera* _camera){

    }

    //This is a virtual function used to invoke the BEAM Garbage Collector.
    virtual void OnInvokeGarbageCollector(){
        
    }
    
    void InvokeGarbageCollector(){
        OnInvokeGarbageCollector();
        for(const auto& actor : actors){
            actor->InvokeGarbageCollector();
        }
    }

    template<typename tActor>
    tActor* DynamicCast(){
        return dynamic_cast<tActor*>(this);
    }

    template<typename tActor>
    tActor* StaticCast(){
        return static_cast<tActor*>(this);
    }

    virtual ~Actor(){
        
    }

    //For UFO-Engine Studio Editor actor tree widget

    virtual void OnUpdateEditorTree(int _index){

    }

    virtual void UpdateEditorTree(int _index){
        
        bool tree_node_opened = ImGui::TreeNodeEx(std::to_string((int)this).c_str());
        if(tree_node_opened){
        
            OnUpdateEditorTree(_index);

            for(int i = 0; i < actors.size(); i++){
                actors[i]->UpdateEditorTree(i);
            }

            for(int i = actors.size()-1; i != -1; i--){
                //if(actors[i]->to_be_moved){

                //}
            }
            
            ImGui::TreePop();
        }
    }

    class EditorAttribute{
    public:
        std::string variable_name;
        std::string alias;
    };

    class EditorAttributeInt : public EditorAttribute{
    public:
        int value;

        EditorAttributeInt(int _value) : value{_value}{}

    };

    class EditorAttributeIntSlider : public EditorAttribute{

    };

    class EditorAttributeIntRange : public EditorAttribute{

    };

    class EditorAttributeOptionStringDropDownMenu : public EditorAttribute{

    };

    class EditorAttributeOptionStringRadioButton : public EditorAttribute{

    };

    class EditorAttributeFloat : public EditorAttribute{

    };

    class EditorAttributeFloatSlider : public EditorAttribute{

    };

    class EditorAttributeFloatRange : public EditorAttribute{

    };

    class EditorAttributeVector2f : public EditorAttribute{
    public:
        Vector2f v;

        EditorAttributeVector2f(Vector2f _v) : v{_v}{}

    };

    std::vector<std::unique_ptr<EditorAttribute>> editor_attributes;

    EditorAttributeVector2f* editor_attribute_local_position = nullptr;

    void UpdateDefaultAttributes(){
        editor_attribute_local_position->v = local_position;
    }

};