#pragma once
#include <exception>
#include <level.h>
#include <stdexcept>
#include <string>
#include <memory>
#include <vector>
#include "background_sprite.h"
#include "file_node.h"
#include "tab.h"
#include <graphics.h>
#include <engine.h>
#include <map>
#include <functional>
#include "../ufo_garbage_collector/object.h"
#include "../src/actor.h"
#include <gc_json.h>
#include "../src/actor.h"
#include "../src/sprite.h"
#include "../src/camera.h"
#include "../ufo_maths/ufo_maths.h"
#include "../tilemap/tile_map.h"
#include "../src/text.h"
#include "../src/widget.h"
#include "../src/animation.h"
#include "../src/button.h"
#include "../src/platformer_rectangle_collision.h"
#include "../src/collision_grid.h"
#include "../src/editor_property.h"
#include "../src/background_sprite.h"
#include "../src/rectangular_area.h"
#include "advanced_actor_spawner.h"
#include "error_dialogue.h"

namespace UFOEngineStudio{

struct ProjectSettings{
    bool v_sync = true;
    int game_width = 1600;
    int game_height = 900;
    std::string game_window_title = "Untitled";
    bool multi_player = false;
};

class Editor : public ufo::Level, public ufo::gc::Root{
public:

    //Stuff for project class.

    std::string open_file_dialog_path = "";
    bool is_open_file_dialog_open = false;

    bool view_calculator = false;
    std::string calculator_expression;

    ProjectSettings project_settings;

    //The current working directory for the projekt. Not for this program.
    std::string opened_directory_path = "";

    //Things that could be included in the new Project class, or at least should disappear when a new project is opened

    std::map<std::string, std::unique_ptr<AdvancedActorSpawner>> spawnable_actor_map;

    std::unique_ptr<FileNode> opened_directory = nullptr;
    std::vector<std::unique_ptr<Tab>> tabs;
    Tab* active_tab = nullptr;

    std::string currently_selected_actor_type = "";

    int actor_count_for_naming_purposes = 0;

    //Editor stuff

    std::unique_ptr<ErrorDialogue> error_dialogue = std::make_unique<ErrorDialogueNoError>();

    bool will_compile_game = false;
    bool will_run_game = false;

    bool project_settings_open = false;

    //The path to the UFO-Engine Header Tool & Boilerplate generator
    std::string header_tool_parser = "ufo_engine_header_tool.py";

    bool refresh_entire_project = false;
    bool set_all_actors_properties_open_to_false = false;

    Editor();
    void ResetUFOEngineStudio();
    void OpenFolder(std::string _path);
    void RefreshFolder();
    void Load();
    void ImportHeaderFileToProject(std::string _path);
    void OnUpdate(float _delta_time);
    void PopulateSpawnableActorMapWithBaseObjects();
    void ReloadSpawnableActorMap();
    void OnMark();

};

void BuildAndRunProgram(const std::string& _build_directory, const std::string& _opened_directory_path);
void DebugGame(const std::string& _build_directory, const std::string& _opened_directory_path);
void RunGame(const std::string& _build_directory, const std::string& _opened_directory_path);

}
