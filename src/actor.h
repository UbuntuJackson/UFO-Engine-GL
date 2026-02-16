#pragma once
#include <memory>
#include <vector>
#include <string>
#include "../ufo_maths/ufo_maths.h"
#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "../ufo_garbage_collector/gc_json.h"
#include "actor_undo_and_redo.h"

class TileMap;

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
    Vector2f former_local_position;

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

    //Adding actors that are added by AddActor and queued in new_actor_queue
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

    Actor* ReplaceActorUniquePtr(int _index,std::unique_ptr<Actor> _u_actor){
        auto actor_ptr = _u_actor.get();
        actor_ptr->parent = this;
        //actor_ptr->level = level;
        OnAddActor(actor_ptr);
        //actor_ptr->engine = engine;
        //actor_ptr->OnSpawn();
        actors.at(_index) = std::move(_u_actor);
        //new_actor_queue.push_back(std::move(_u_actor));
        return actor_ptr;
    }

    //Actor(const Actor&) = delete;
    //Actor(Actor&&) = delete;

    void CleanUpDeadActors();
    void StashActors();

    void MarkAllDead();

    virtual void OnSpawn();

    virtual void OnAddActor(Actor* _actor);

    virtual void Update(float _delta_time);

    virtual void OnUpdate(float _delta_time){

    }

    void IrregularUpdate();

    virtual void OnIrregularUpdate();

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

    enum ImportModes{
        WRAPPED,
        MODIFIABLE,
        UNWRAPPED
    };

    int import_mode = ImportModes::UNWRAPPED;

    bool is_savable = true;

    static inline int editor_id_counter = 0;
    int editor_id = 0;

    struct InsertedActor{
        int index;
        std::unique_ptr<Actor> actor;
    };

    bool is_selected_in_viewport = false;

    //Rename this is_selected_in_actor_tree ?
    bool is_selected = false;

    void AddToLevelEditorTabIfSelected(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index);

    bool should_be_selected = false;

    void ResetSelectionStatus();

    std::vector<InsertedActor> inserted_actor_queue;

    void InsertActors();

    void InsertActorUniquePtr(std::unique_ptr<Actor>&& _ptr, const int _index);

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

    std::string GetImportStatus();

    bool is_imported = false; void DeclareImportedRecursive(); void UpdateActorStructure(UFOEngineStudio::Editor* _editor, bool  _parent_is_modifiable);

    bool adding_new_actor = false;

    TileMap* IsInTileMap();

    virtual void UpdateEditorTree(UFOEngineStudio::Editor* _editor, int _index);

    class EditorProperty{
    public:
        std::string variable_name;
        std::string alias;
        bool to_be_removed = false;
        bool viewport_text = false;

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

    class EditorPropertyString : public EditorProperty{
    public:
        std::string value;

        EditorPropertyString(const std::string& _name,const std::string& _alias, const std::string& _value) : EditorProperty(_name,_alias),
            value{_value}{

            }

        void Update(const std::string& _editor_name, int _index){
            ImGui::InputText(std::string(alias+"###Property"+_editor_name+std::to_string(_index)).c_str(), &value);
        }

        ufo::gc::JsonMap* GetJson(ufo::GarbageCollector* _gc){
            auto m = _gc->New<ufo::gc::JsonMap>();
            m->map.emplace("name", _gc->New<ufo::gc::JsonString>(variable_name));
            m->map.emplace("type", _gc->New<ufo::gc::JsonString>("std::string"));
            m->map.emplace("value", _gc->New<ufo::gc::JsonString>(value));
            m->map.emplace("hint", _gc->New<ufo::gc::JsonString>("EditorPropertyString"));
            return m;
        }

        std::unique_ptr<EditorProperty> Copy(){
            return std::make_unique<EditorPropertyString>(variable_name,alias,value);
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

    class EditorPropertyFloatSlider : public EditorProperty{
    public:
        float value;

        float min;
        float max;
        float step;

        EditorPropertyFloatSlider(const std::string& _name,const std::string& _alias, float _value, float _min, float _max, float _step) : EditorProperty(_name,_alias),
            value{_value},
            min{_min},
            max{_max},
            step{_step}{

            }

        void Update(const std::string& _editor_name, int _index){
            ImGui::SliderFloat(std::string(alias+"###Property"+_editor_name+std::to_string(_index)).c_str(), &value, min, max);
        }

        ufo::gc::JsonMap* GetJson(ufo::GarbageCollector* _gc){
            auto m = _gc->New<ufo::gc::JsonMap>();
            m->map.emplace("name", _gc->New<ufo::gc::JsonString>(variable_name));
            m->map.emplace("type", _gc->New<ufo::gc::JsonString>("float"));
            m->map.emplace("value", _gc->New<ufo::gc::JsonNumber>(value));
            m->map.emplace("hint", _gc->New<ufo::gc::JsonString>("EditorPropertyFloatSlider"));
            return m;
        }

        std::unique_ptr<EditorProperty> Copy(){
            return std::make_unique<EditorPropertyFloatSlider>(variable_name,alias,value,min,max, step);
        }
    };

    class EditorPropertyCheckBox : public EditorProperty{
    public:
        bool value = false;

        EditorPropertyCheckBox(const std::string& _name,const std::string& _alias, bool _value) : EditorProperty(_name,_alias),
        value{_value}{

        }

        void Update(const std::string& _editor_name, int _index){
            ImGui::Checkbox(std::string(alias+"###Property"+_editor_name+std::to_string(_index)).c_str(), &value);
        }

        ufo::gc::JsonMap* GetJson(ufo::GarbageCollector* _gc){
            auto m = _gc->New<ufo::gc::JsonMap>();
            m->map.emplace("name", _gc->New<ufo::gc::JsonString>(variable_name));
            m->map.emplace("type", _gc->New<ufo::gc::JsonString>("bool"));
            m->map.emplace("value", _gc->New<ufo::gc::JsonNumber>(value));
            m->map.emplace("hint", _gc->New<ufo::gc::JsonString>("EditorPropertyCheckBox"));
            return m;
        }

        std::unique_ptr<EditorProperty> Copy(){
            return std::make_unique<EditorPropertyCheckBox>(variable_name,alias,value);
        }
    };

    ufo::Rectangle editor_hitbox = ufo::Rectangle(Vector2f(-6.0f, -6.0f),Vector2f(12.0f, 12.0f));

    std::vector<std::unique_ptr<EditorProperty>> editor_properties;

    void RemoveAndAddEditorPropertiesDuringRuntime(UFOEngineStudio::Editor* _editor);

    bool should_open_properties = false;

    bool properties_open = false;

    std::string find_actor_search_field = "";

    virtual void OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index);

    void ViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index);

    void OpenProperties();

    void GetSelectedActors(std::vector<Actor*>& _selected_actors, ufo::Rectangle _selection_rectangle_world_space){
        if(!ImGui::IsKeyDown(ImGuiKey_LeftShift)) is_selected_in_viewport = false;

        if(ufoMaths::RectangleVsPoint(_selection_rectangle_world_space , GetGlobalPosition())){
            _selected_actors.push_back(this);
        }

        for(const auto& actor : actors){
            actor->GetSelectedActors(_selected_actors, _selection_rectangle_world_space);
        }
    }

    virtual void OnAdditionalButtonsForTreeItem();

    bool is_grabbed_by_cursor = false;
    void UpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    bool UpdateEditorViewportFocus(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    virtual void OnSelectedInViewport(UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    virtual bool OnUpdateEditorViewportFocus(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    std::string editor_viewport_text;
    virtual void OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    virtual void OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    void DrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    virtual ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc);
    virtual void OnLoadDefaultProperties(ufo::gc::JsonMap* _json);

    //stashing the actor means you basically get the actor out of the tree but keeping it in memory
    bool stash = false;

protected:
//To determine whether actor is the container node for all other actors
    bool is_top_actor_in_editor = false;
    bool unremovable = false;
    friend class UFOEngineStudio::LevelEditorTab;

};
