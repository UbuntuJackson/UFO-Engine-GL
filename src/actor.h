#pragma once
#include <memory>
#include <vector>
#include <string>
#include "../ufo_maths/ufo_maths.h"
#include "graphics.h"
#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include <gc_json.h>

namespace ufo{
    class Engine;
    class GarbageCollector;
}

namespace ufo::gc{
    class JsonMap;
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

        bool queue_was_empty = new_actor_queue.size() == 0;

        for(auto&& actor : new_actor_queue){
            actors.push_back(std::move(actor));
        }

        for(auto&& actor : actors){
            actor->AddNewActors();
        }

        new_actor_queue.clear();
    
        if(!queue_was_empty && !should_be_sorted){
            
            for(int i = 0; i < actors.size(); i++){
                actors[i]->order_index = i;
            }
        }
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

    static inline int editor_id_counter = 0;
    int editor_id = 0;

    struct InsertedActor{
        int index;
        std::unique_ptr<Actor> actor;
    };

    std::vector<InsertedActor> inserted_actor_queue;
    
    void InsertActors(){
        for(auto&& inserted_actor : inserted_actor_queue){
            actors.insert(actors.begin()+inserted_actor.index, std::move(inserted_actor.actor));
        }

        inserted_actor_queue.clear();

        for(const auto& actor : actors){
            actor->InsertActors();
        }
    }

    int order_index = 0;
    bool should_be_sorted = false;

    void SetOrderIndex(int _index){
        if(parent) parent->should_be_sorted = true;
        order_index = _index;
    }

    void SortActors(){
        std::sort(actors.begin(), actors.end(), [this](const auto& _a, const auto& _b){
            return _a->order_index < _b->order_index;
        });

        for(int i = 0; i < actors.size(); i++){
            actors[i]->order_index = i;
        }

        should_be_sorted = false;
    }

    std::string editor_name = "Actor";
    std::string class_name = "Actor";

    bool marked_for_drag = false;
    bool marked_for_drop = false;

    virtual void OnUpdateEditorTree(int _index){

    }

    struct DraggedActorWhereAbouts{
        Actor* parent;
        int index;
    };

    DraggedActorWhereAbouts dragged_actor_where_abouts;

    bool editing_name = false;
    std::string old_editor_name = "";

    void TurnOnEditMode(){
        editing_name = true;
        old_editor_name = editor_name;
    }

    bool adding_new_actor = false;

    virtual void UpdateEditorTree(int _index);

    virtual void InitEditorProperties(){
        editor_properties.push_back(std::make_unique<EditorPropertyFloatHandle>("x","x",&(local_position.x)));
        editor_properties.push_back(std::make_unique<EditorPropertyFloatHandle>("y","y",&(local_position.y)));
        editor_properties.push_back(std::make_unique<EditorPropertyIntSlider>("_IntSlider","IntSlider",5, 0, 10));
    }

    class EditorProperty{
    public:
        std::string variable_name;
        std::string alias;

        EditorProperty(const std::string& _variable_name, const std::string& _alias) : variable_name{_variable_name}, alias{_alias}{}

        virtual ufo::gc::JsonMap* GetJson(ufo::GarbageCollector* _gc){
            return nullptr;
        }

        virtual void Update(const std::string& _editor_name, int _index){

        }
    };

    class EditorPropertyFloatHandle : public EditorProperty{
    public:
        float* value = nullptr;

        EditorPropertyFloatHandle(const std::string& _name,const std::string& _alias, float* _value) : EditorProperty(_name,_alias), value{_value}{}

        void Update(const std::string& _editor_name, int _index){
            ImGui::InputFloat(std::string(alias+"###Property"+_editor_name+std::to_string(_index)).c_str(), value);
        }

        ufo::gc::JsonMap* GetJson(ufo::GarbageCollector* _gc){
            auto m = _gc->New<ufo::gc::JsonMap>();
            m->map.emplace("name", _gc->New<ufo::gc::JsonString>(variable_name));
            m->map.emplace("type", _gc->New<ufo::gc::JsonString>("float"));
            m->map.emplace("value", _gc->New<ufo::gc::JsonNumber>(*value));
            return m;
        }
    };

    class EditorPropertyIntSlider : public EditorProperty{
    public:
        int value = 0;

        int min;
        int max;

        EditorPropertyIntSlider(const std::string& _name,const std::string& _alias, int _value, int _min, int _max) : EditorProperty(_name,_alias),
            value{_value},
            max{_max},
            min{_min}{

            }

        void Update(const std::string& _editor_name, int _index){
            ImGui::SliderInt(std::string(alias+"###Property"+_editor_name+std::to_string(_index)).c_str(), &value, min, max);
        }

        ufo::gc::JsonMap* GetJson(ufo::GarbageCollector* _gc){
            auto m = _gc->New<ufo::gc::JsonMap>();
            m->map.emplace("name", _gc->New<ufo::gc::JsonString>(variable_name));
            m->map.emplace("type", _gc->New<ufo::gc::JsonString>("float"));
            m->map.emplace("value", _gc->New<ufo::gc::JsonNumber>(value));
            return m;
        }
    };

    /*class EditorPropertyIntSlider : public EditorProperty{

    };

    class EditorPropertyIntRange : public EditorProperty{

    };

    class EditorPropertyOptionStringDropDownMenu : public EditorProperty{

    };

    class EditorPropertyOptionStringRadioButton : public EditorProperty{

    };

    class EditorPropertyFloat : public EditorProperty{

    };

    class EditorPropertyFloatSlider : public EditorProperty{

    };

    class EditorPropertyFloatRange : public EditorProperty{

    };

    class EditorPropertyVector2f : public EditorProperty{
    public:
        Vector2f v;

        EditorPropertyVector2f(Vector2f _v) : v{_v}{}

    };

    EditorPropertyVector2f* editor_attribute_local_position = nullptr;*/


    std::vector<std::unique_ptr<EditorProperty>> editor_properties;

    bool properties_open = false;

    virtual void OnViewProperties(int _index);

    void ViewProperties(int _index){
        if(properties_open) OnViewProperties(_index);
        for(int i = 0; i < actors.size(); i++){
            actors[i]->ViewProperties(i);
        }
    }

    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc);

};