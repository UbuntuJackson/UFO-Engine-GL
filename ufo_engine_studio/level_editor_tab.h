#pragma once
#include "../ufo_engine_studio/tab.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui.h"
#include "../ufo_garbage_collector/garbage_collector.h"
#include "../src/actor.h"
#include <vector>

class Level;

namespace ufo{
    class Engine;
    class Graphics;
    class Editor;
}

namespace UFOEngineStudio{

class LevelEditorTab : public Tab{
public:
    enum Tools{ SELECT, PLACE, ERASE };

    Tools current_tool = Tools::SELECT;

    ufo::Engine* engine = nullptr;

    Actor* spawn_cursor = nullptr;
    Level* this_level = nullptr;
    std::string currently_viewed_properties_actor_name;
    float window_to_engine_ratio = 1.0f;
    std::unique_ptr<Actor> replace_with_actor;
    std::string header_file;

    Vector2f level_viewport_position;

    Vector2f mouse_position_over_screenspace;
    Vector2f former_mouse_position_over_screenspace;

    std::vector<Actor::DraggedActorWhereAbouts> drag_dropped_actors;

    bool reset_selection_status = false;

    LevelEditorTab(ufo::Engine* _engine, Editor* _editor);

    Vector2f TranslateToEditorScreenSpace(Vector2f _position);

    void Initialise();

    void Refresh();

    void OnActive(ImGuiID _local_dockspace_id , Editor* _editor, float _delta_time);

    void OnMakeDockSpace(ImGuiID _local_dockspace_id, Editor* _program_state);

    void LevelUpdatePhase(float _delta_time);

    void LevelDrawPhase(ufo::Graphics* _graphics);

    void OnSave(Editor* _editor);
};

}
