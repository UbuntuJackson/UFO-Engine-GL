#pragma once
#include <memory>
#include <vector>
#include <string>
#include "../ufo_maths/ufo_maths.h"
#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "../ufo_garbage_collector/gc_json.h"

namespace UFOEngineStudio{
    class Editor;
    class LevelEditorTab;
}

namespace ufo{
    namespace gc{
        class JsonMap;
    }
    class Engine;
    class GarbageCollector;
    class Graphics;
}

class Level;

//Using rule of 0 3 and 5 for all actors
class Actor{
public:

    Vector2f local_position;

    bool is_dead = false;

    Actor(Vector2f _local_position);
public:
    Actor* parent = nullptr;
public:
    ufo::Engine* engine = nullptr;
    Level* level = nullptr;

    Vector2f GetGlobalPosition();

    Actor* GetActor(std::string _path);

    //Reserve a set number of actors?
    std::vector<std::unique_ptr<Actor>> actors;

    std::vector<std::unique_ptr<Actor>> new_actor_queue;

    void AddNewActors();

    template <typename tActor, typename ...tArgs>
    tActor* AddActor(tArgs&& ..._args){
        auto u_actor = std::make_unique<tActor>(_args...);
        auto actor_ptr = u_actor.get();
        actor_ptr->parent = this;
        //actor_ptr->level = level;
        OnAddActor(actor_ptr);
        //actor_ptr->engine = engine;
        //actor_ptr->OnSpawn();
        new_actor_queue.push_back(std::move(u_actor));
        return actor_ptr;
    }

    Actor* AddActorUniquePtr(std::unique_ptr<Actor> _u_actor){
        auto actor_ptr = _u_actor.get();
        actor_ptr->parent = this;
        //actor_ptr->level = level;
        OnAddActor(actor_ptr);
        //actor_ptr->engine = engine;
        //actor_ptr->OnSpawn();
        new_actor_queue.push_back(std::move(_u_actor));
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

    void WidgetDraw(ufo::Graphics* _graphics);

    virtual void OnWidgetDraw(ufo::Graphics* _graphics);

    void Draw(ufo::Graphics* _graphics, Camera* _camera);

    virtual void OnDraw(ufo::Graphics* _graphics, Camera* _camera);

    //This is a virtual function used to invoke the BEAM Garbage Collector.
    virtual void OnInvokeGarbageCollector();

    void InvokeGarbageCollector();

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

    void InsertActors();

    int order_index = 0;
    bool should_be_sorted = false;

    void SetOrderIndex(int _index);

    void SortActors();

    bool to_replace = false;
    void ReplaceActors(UFOEngineStudio::Editor* _editor);

    std::string editor_name = "Actor";
    std::string class_name = "Actor";
    std::string base_class_name = "Actor";

    bool marked_for_drag = false;
    bool marked_for_drop = false;

    virtual void OnUpdateEditorTree(int _index);

    struct DraggedActorWhereAbouts{
        Actor* parent;
        int index;
    };

    DraggedActorWhereAbouts dragged_actor_where_abouts;

    bool editing_name = false;
    std::string old_editor_name = "";

    void TurnOnEditMode();

    bool adding_new_actor = false;

    virtual void UpdateEditorTree(UFOEngineStudio::Editor* _editor, int _index);

    virtual void InitEditorProperties();

    class EditorProperty{
    public:
        std::string variable_name;
        std::string alias;
        bool to_be_removed = false;

        EditorProperty(const std::string& _variable_name, const std::string& _alias) : variable_name{_variable_name}, alias{_alias}{}

        virtual ufo::gc::JsonMap* GetJson(ufo::GarbageCollector* _gc){
            return nullptr;
        }

        virtual void Update(const std::string& _editor_name, int _index){

        }

        virtual std::unique_ptr<EditorProperty> Copy() = 0;

        virtual ~EditorProperty() = default;
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

        std::unique_ptr<EditorProperty> Copy(){
            return std::make_unique<EditorPropertyFloatHandle>(variable_name,alias,value);
        }
    };

    class EditorPropertyIntHandle : public EditorProperty{
    public:
        int* value = nullptr;

        EditorPropertyIntHandle(const std::string& _name,const std::string& _alias, int* _value) : EditorProperty(_name,_alias), value{_value}{}

        void Update(const std::string& _editor_name, int _index){
            ImGui::InputInt(std::string(alias+"###Property"+_editor_name+std::to_string(_index)).c_str(), value);
        }

        ufo::gc::JsonMap* GetJson(ufo::GarbageCollector* _gc){
            auto m = _gc->New<ufo::gc::JsonMap>();
            m->map.emplace("name", _gc->New<ufo::gc::JsonString>(variable_name));
            m->map.emplace("type", _gc->New<ufo::gc::JsonString>("float"));
            m->map.emplace("value", _gc->New<ufo::gc::JsonNumber>(*value));
            return m;
        }

        std::unique_ptr<EditorProperty> Copy(){
            return std::make_unique<EditorPropertyIntHandle>(variable_name,alias,value);
        }
    };

    class EditorPropertyInt : public EditorProperty{
    public:
        int value = 0;

        EditorPropertyInt(const std::string& _name,const std::string& _alias, int _value) : EditorProperty(_name,_alias),
            value{_value}{

            }

        void Update(const std::string& _editor_name, int _index){
            ImGui::InputInt(std::string(alias+"###Property"+_editor_name+std::to_string(_index)).c_str(), &value);
        }

        ufo::gc::JsonMap* GetJson(ufo::GarbageCollector* _gc){
            auto m = _gc->New<ufo::gc::JsonMap>();
            m->map.emplace("name", _gc->New<ufo::gc::JsonString>(variable_name));
            m->map.emplace("type", _gc->New<ufo::gc::JsonString>("float"));
            m->map.emplace("value", _gc->New<ufo::gc::JsonNumber>(value));
            m->map.emplace("hint", _gc->New<ufo::gc::JsonString>("EditorPropertyInt"));
            return m;
        }

        std::unique_ptr<EditorProperty> Copy(){
            return std::make_unique<EditorPropertyInt>(variable_name,alias,value);
        }
    };

    class EditorPropertyFloat : public EditorProperty{
    public:
        float value = 0;

        EditorPropertyFloat(const std::string& _name,const std::string& _alias, float _value) : EditorProperty(_name,_alias),
            value{_value}{

            }

        void Update(const std::string& _editor_name, int _index){
            ImGui::InputFloat(std::string(alias+"###Property"+_editor_name+std::to_string(_index)).c_str(), &value);
        }

        ufo::gc::JsonMap* GetJson(ufo::GarbageCollector* _gc){
            auto m = _gc->New<ufo::gc::JsonMap>();
            m->map.emplace("name", _gc->New<ufo::gc::JsonString>(variable_name));
            m->map.emplace("type", _gc->New<ufo::gc::JsonString>("float"));
            m->map.emplace("value", _gc->New<ufo::gc::JsonNumber>(value));
            m->map.emplace("hint", _gc->New<ufo::gc::JsonString>("EditorPropertyFloat"));
            return m;
        }

        std::unique_ptr<EditorProperty> Copy(){
            return std::make_unique<EditorPropertyFloat>(variable_name,alias,value);
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
            m->map.emplace("hint", _gc->New<ufo::gc::JsonString>("EditorPropertyIntSlider"));
            return m;
        }

        std::unique_ptr<EditorProperty> Copy(){
            return std::make_unique<EditorPropertyIntSlider>(variable_name,alias,value,min,max);
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

    void RemoveAndAddEditorPropertiesDuringRuntime(UFOEngineStudio::Editor* _editor);

    bool should_open_properties = false;

    bool properties_open = false;

    std::string find_actor_search_field = "";

    virtual void OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index);

    void ViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index);

    void OpenProperties();

    void UpdateEditorViewport(UFOEngineStudio::Editor* _editor);
    virtual void OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor);

    virtual ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc);

};
