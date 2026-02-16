#pragma once

#include <vector>
#include <memory>
#include "actor.h"
#include "../tilemap/tileset_manager.h"
#include "actor_undo_and_redo.h"
#include "widget.h"

class Camera;

namespace ufo{
    class Controls;
    class Graphics;
}

class Level : public Actor{
public:
    TilesetManager tileset_manager;

    Vector2f size = {20000.0f, 3000.0f};

    enum CleanUpModes{
        EACH_FRAME,
        CYCLICAL
    };

    std::unique_ptr<ufo::Widget> widget = std::make_unique<ufo::Widget>(Vector2f(0.0f, 0.0f));

    std::vector<int> deletion_history;

    std::vector<Camera*> camera_handles;
    std::vector<Camera*> active_camera_handles;
private:
    int clean_up_mode = CleanUpModes::EACH_FRAME;
public:
    Level();
    void OnSpawn();
private:
    int control_handout_counter = -1;
public:
    ufo::Controls* GetControls();

    //If you want the bare minimum for a functioning level
    virtual void LoadBlank(){

    }

    virtual void Load();

    void OnAddActor(Actor* _actor){
        _actor->level = this;
    }

    void LoadEngineObjects(/*JsonUnion _level_file*/){/**/}

    virtual void LoadCustomObjects(/*JsonUnion _level_file*/){
        //Generated::AssembleCustomActors(_level_file);
    }

    virtual ~Level(){}

    void Update(float _delta_time);

    void UpdatePhase(float _delta_time);

    std::vector<std::unique_ptr<Actor>> stashed_actors;

    void Undo();
    void Redo();
    void RemoveFutureChanges();

    void OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index);

    void OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    bool OnUpdateEditorViewportFocus(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    void OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    void DrawPhase(ufo::Graphics* _graphics);

    virtual void EditorUpdatePhase(float _delta_time){

    }

    virtual void EditorDrawPhase(){

    }

    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc){

        ufo::gc::JsonMap* parent_class_as_json = Actor::GetAsJson(_gc);
        ufo::gc::JsonArray* tilesets = _gc->New<ufo::gc::JsonArray>();

        for(const auto& tileset : tileset_manager.tileset_data){
            if(!tileset.is_loaded_from_path) continue;
            ufo::gc::JsonMap* j_tileset = _gc->New<ufo::gc::JsonMap>();
            j_tileset->map.emplace("name",_gc->New<ufo::gc::JsonString>(tileset.name));
            j_tileset->map.emplace("columns",_gc->New<ufo::gc::JsonNumber>(tileset.columns));
            j_tileset->map.emplace("tileset_start_id",_gc->New<ufo::gc::JsonNumber>(tileset.tileset_start_id));
            j_tileset->map.emplace("image_width",_gc->New<ufo::gc::JsonNumber>(tileset.image_width));
            j_tileset->map.emplace("image_height",_gc->New<ufo::gc::JsonNumber>(tileset.image_height));
            j_tileset->map.emplace("tile_width",_gc->New<ufo::gc::JsonNumber>(tileset.tile_width));
            j_tileset->map.emplace("tile_height",_gc->New<ufo::gc::JsonNumber>(tileset.tile_height));
            j_tileset->map.emplace("tile_count",_gc->New<ufo::gc::JsonNumber>(tileset.tile_count) );
            tilesets->array.push_back(j_tileset);
        }

        parent_class_as_json->map.emplace("tilesets",tilesets);

        parent_class_as_json->map.emplace("size_x",_gc->New<ufo::gc::JsonNumber>(size.x));
        parent_class_as_json->map.emplace("size_y",_gc->New<ufo::gc::JsonNumber>(size.y));

        return parent_class_as_json;
    }

    void DrawGizmosPhase(ufo::Graphics* _graphics, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    int current_level_change = -1;

    std::vector<std::unique_ptr<ufo::ActorChange>> level_changes;

};
