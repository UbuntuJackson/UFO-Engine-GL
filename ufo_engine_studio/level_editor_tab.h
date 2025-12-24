#pragma once
#include "../ufo_engine_studio/tab.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui.h"
#include "../ufo_garbage_collector/garbage_collector.h"
#include "../src/actor.h"

class Level;

namespace ufo{
    class Engine;
    class Graphics;
    class Editor;
}

namespace UFOEngineStudio{

class LevelEditorTab : public Tab{
public:
    ufo::Engine* engine = nullptr;
    Level* this_level = nullptr;
    std::string currently_viewed_properties_actor_name;
    std::unique_ptr<Actor> replace_with_actor;
    std::string header_file;

    LevelEditorTab(ufo::Engine* _engine, Editor* _editor);

    void Initialise();

    void Refresh();

    void OnActive(ImGuiID _local_dockspace_id , Editor* _editor, float _delta_time);

    void OnMakeDockSpace(ImGuiID _local_dockspace_id, Editor* _program_state);

    void LevelUpdatePhase(float _delta_time);

    void LevelDrawPhase(ufo::Graphics* _graphics);

    void OnSave(Editor* _editor);
};

}
