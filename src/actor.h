#pragma once
#include <memory>
#include <vector>
#include <string>
#include "../ufo_maths/ufo_maths.h"
#include "../shapes/rectangle.h"

#ifdef UFO_ENGINE_STUDIO
#include "actor_undo_and_redo.h"
#include "editor_property.h"
#include "../imgui/imgui.h"
#endif //UFO_ENGINE_STUDIO

namespace UFOEngineStudio{
    class Editor;
    class LevelEditorTab;
}

namespace ufo{

namespace gc{
    class JsonMap;
}

class TileMap;
class Engine;
class GarbageCollector;
class Graphics;
class Level;
class Camera;

//Using rule of 0 3 and 5 for all actors
class Actor{
public:

    std::string editor_name = "Actor";
    std::string class_name = "ufo::Actor";
    std::string base_class_name = "ufo::Actor";

    Vector2f local_position;
    Vector2f former_local_position;

    ufo::Rectangle rectangle = ufo::Rectangle(Vector2f(0.0f, 0.0f), Vector2f(32.0f, 32.0f));

    ufo::Rectangle former_rectangle = ufo::Rectangle(Vector2f(0.0f, 0.0f),Vector2f(0.0f, 0.0f));

    bool is_dead = false;

    Actor(Vector2f _local_position);

    virtual std::string GetInfo();

public:
    Actor* parent = nullptr;
public:
    ufo::Engine* engine = nullptr;
    Level* level = nullptr;

    Vector2f GetGlobalPosition();

    virtual ufo::Rectangle GetRectangle();
    virtual ufo::Rectangle GetLocalRectangle();

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
        new_actor_queue.push_back(std::move(u_actor));
        return actor_ptr;
    }

    Actor* AddActorUniquePtr(std::unique_ptr<Actor> _u_actor){
        auto actor_ptr = _u_actor.get();
        actor_ptr->parent = this;
        new_actor_queue.push_back(std::move(_u_actor));
        return actor_ptr;
    }

    //Unused
    Actor* ReplaceActorUniquePtr(int _index,std::unique_ptr<Actor> _u_actor){
        auto actor_ptr = _u_actor.get();
        actor_ptr->parent = this;
        //actor_ptr->level = level;
        //OnAddActor(actor_ptr);
        //actor_ptr->engine = engine;
        //actor_ptr->OnSpawn();
        actors.at(_index) = std::move(_u_actor);
        //new_actor_queue.push_back(std::move(_u_actor));
        return actor_ptr;
    }

    Actor(const Actor&) = delete;
    Actor(Actor&&) = delete;

    void CleanUpDeadActors();
    void StashActors();
    void MarkAllDead();
    virtual void OnKilled();

    virtual void OnSpawn();

    virtual void Update(float _delta_time);

    virtual void OnUpdate([[maybe_unused]] float _delta_time){}

    virtual void Pause(float _delta_time);
    virtual void OnPause([[maybe_unused]] float _delta_time){}

    void IrregularUpdate();

    virtual void OnIrregularUpdate();

    void WidgetDraw(ufo::Graphics* _graphics);

    virtual void OnWidgetDraw(ufo::Graphics* _graphics);

    virtual void Draw(ufo::Graphics* _graphics, Camera* _camera);

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

    struct MovedActor{
        ufo::Actor* actor_to_move;
        ufo::Actor* original_parent;
        int original_index;
        ufo::Actor* move_to_parent;
        int move_to_index;
    };

    void EnumerateActorsAnew();

    int order_index = 0;
    bool should_be_sorted = false;

    void SetOrderIndex(int _index);

    void SortActors();

    bool is_editor_hit_box_unique_per_instance = false;
    ufo::Rectangle GetEditorHitBox();

    virtual ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc);
    virtual void OnLoadDefaultProperties(ufo::gc::JsonMap* _json);

    //For UFO-Engine Studio Editor actor tree widget

    bool adding_new_actor = false;

    TileMap* GetTileMap();

    enum ImportModes{
        CUSTOM_CLASS,
        VARIATION_OF_CUSTOM_CLASS,
        BUILT_IN_CLASS
    };

    int import_mode = ImportModes::BUILT_IN_CLASS;

    static inline int editor_id_counter = 0;
    int editor_id = 0;

ufo::Rectangle editor_hitbox = ufo::Rectangle(Vector2f(-6.0f, -6.0f),Vector2f(12.0f, 12.0f));

#ifdef UFO_ENGINE_STUDIO

    //Handles to member variables independent of memory address
    std::map<std::string,Vector2f*> vector2f_handles;
    std::map<std::string,std::string*> string_handles;
    std::map<std::string,int*> int_handles;
    std::map<std::string,float*> float_handles;

    virtual bool OnEndUndoRedoAction([[maybe_unused]] UFOEngineStudio::LevelEditorTab* _level_editor_tab){return false;}

    virtual void OnResourcesEdited(){

    }

    void ResourcesEdited(){
        OnResourcesEdited();
        for(const auto& actor : actors){
            actor->ResourcesEdited();
        }
    }

    void SetVector2fUndoAndRedo(UFOEngineStudio::LevelEditorTab* _level_editor_tab, std::string _name, Vector2f _value);

    bool is_savable = true;

    bool is_selected_in_viewport = false;

    //Rename this is_selected_in_actor_tree ?
    bool is_selected = false;

    void AddToLevelEditorTabIfSelected(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index);

    bool should_be_selected = false;

    void ResetSelectionStatus();

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

    void ReplaceCustomActors(UFOEngineStudio::Editor* _editor, std::vector<std::unique_ptr<Actor>>& _actor_queue);
    void UpdateActorStructure(UFOEngineStudio::Editor* _editor);

    void DragDropMiddleButton(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index);
    virtual void UpdateEditorTree(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index);

    std::vector<std::unique_ptr<ufo::EditorProperty>> editor_properties;

    void RemoveAndAddEditorPropertiesDuringRuntime(UFOEngineStudio::Editor* _editor);

    bool properties_open = false;

    std::string find_actor_search_field = "";

    bool InputFloatWithUndoAndRedo(UFOEngineStudio::LevelEditorTab* _level_editor_tab ,const std::string& _id, std::string _name);

    //This function is currently untested
    bool InputIntWithUndoAndRedo(UFOEngineStudio::LevelEditorTab* _level_editor_tab ,const std::string& _id, std::string _name);

    //This function is currently untested
    bool InputTextWithUndoAndRedo(UFOEngineStudio::LevelEditorTab* _level_editor_tab ,const std::string& _id, std::string _name);

    virtual void OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index);

    void ViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index);

    bool is_permanently_non_selectable = false;
    bool is_selectable = true;
    bool IsSelectable();
    void GetSelectedActors(std::vector<int>& _selected_actors, ufo::Rectangle _selection_rectangle_world_space);

    virtual void OnAdditionalButtonsForTreeItem();

    virtual void OnUtiliseAssetManager(UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    //Function dedicated solely for the purpose of getting the ONE hovered actor, if it exists at all
    Actor* GetFocusedActor(Vector2f _cursor_viewport_position);
    virtual Actor* OnGetFocusedActor(Vector2f _cursor_viewport_position);
    virtual void OnHandleSingleSelect(UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    bool is_grabbed_by_cursor = false;
    void UpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    virtual void OnResize(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    virtual void OnSelectedInViewport(UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    std::string editor_viewport_text;
    virtual void OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    virtual void OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    void DrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    //stashing the actor means you basically get the actor out of the tree but keeping it in memory
    bool stash = false;

protected:
//To determine whether actor is the container node for all other actors
    bool is_top_actor_in_editor = false;
    bool unremovable = false;
    friend class UFOEngineStudio::LevelEditorTab;

#endif //UFO_ENGINE_STUDIO

};

void MoveActor(Actor::MovedActor& _inserted_actor);

}
