#pragma once

#include <vector>
#include <memory>
#include "actor.h"
#include "../tilemap/tileset_manager.h"
#include "widget.h"

#ifdef UFO_ENGINE_STUDIO
#include "actor_undo_and_redo.h"
#endif

namespace ufo{

class Camera;
class Controls;
class Graphics;

class Level : public Actor{
public:
    //Keep level in memory on level transition
    bool persistent_on_level_transition = false;

    bool is_paused = false;

    TilesetManager tileset_manager;

    Vector2f size = {20000.0f, 3000.0f};

    enum CleanUpModes{
        EACH_FRAME,
        CYCLICAL
    };

    std::vector<int> deletion_history;

    std::vector<Camera*> active_camera_handles;
private:
    int clean_up_mode = CleanUpModes::EACH_FRAME;
public:
    Level();
    void OnSpawn() override;
private:
    int control_handout_counter = -1;
public:
    ufo::Controls* GetControls();

    //If you want the bare minimum for a functioning level
    virtual void LoadBlank(){

    }

    virtual void Load();

    virtual ~Level(){}

    void Update(float _delta_time) override;

    void UpdatePhase(float _delta_time);

    void DrawPhase(ufo::Graphics* _graphics, int _width, int _height);

    virtual void EditorUpdatePhase([[maybe_unused]] float _delta_time){

    }

    virtual void EditorDrawPhase(){

    }

    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc) override;

#ifdef UFO_ENGINE_STUDIO

    Actor* OnGetFocusedActor(Vector2f _mouse_position_over_screenspace) override;

    std::vector<std::unique_ptr<Actor>> stashed_actors;

    std::map<int, ufo::Actor*> actors_with_stable_id;

    void OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index) override;

    void Undo();
    void Redo();
    void RemoveFutureChanges();

    void OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab) override;

    bool OnUpdateEditorViewportFocus(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    void OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab) override;

    void DrawGizmosPhase(ufo::Graphics* _graphics, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    int current_level_change = -1;

    std::vector<std::unique_ptr<ufo::ActorChange>> level_changes;

#endif

};

}
