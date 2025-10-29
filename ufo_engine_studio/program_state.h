#pragma once
#include <string>
#include <SDL3/SDL.h>
#include <memory>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include "../json/json_variant.h"
#include "tab.h"
#include "file_node.h"
#include "actor_class.h"
#include "actor_node.h"
#include "../json/json_variant.h"
#include "../console/console.h"
#include "asset_manager.h"
#include <thread>
#include "game_engine_thread.h"

namespace UFOEngineStudio{

struct Project{
    
    bool is_valid;
    std::string name;
    
    std::vector<ActorClass> actor_classes;

};

struct DragDrop{
    FileNode* file_to_be_moved = nullptr;
    FileNode* owner = nullptr;
    std::string owner_path;
    FileNode* move_to_folder = nullptr;
    std::string move_to_path;
    int index_in_move_to_folder = 0;
};

class ProgramState{
public:
    GameEngineThread game_engine_thread;

    std::thread* t = nullptr;

    AssetManager asset_manager;

    SDL_Window* window = nullptr;

    Project project = Project{false};
    bool show_new_project_dialogue = false;
    std::string pending_project_name = "";

    bool asset_browser_open = false;

    std::vector<std::unique_ptr<UFOEngineStudio::Tab>> tabs;
    UFOEngineStudio::Tab* active_tab = nullptr;

    bool should_refresh_properties_on_all_nodes = false;

    bool should_refresh_working_directory = false;

    std::string log_text = "[!] Hello from the UFO-Engine Studio log.";

    std::string working_directory_path;
    std::unique_ptr<FileNode> opened_directory;

    //The contents that are being drag dropped. I made this a vector for now, but ideally it should just be a pointer, coupled
    // with a vector of all the contents that are being moved. When you think about it, conventionally you can select multiple
    // files, but you can only drop the in one location. Thus, a pointer to location, and a vector with the contents.
    //Also the ImGui payload is unused since I resorted to implementing drag and drop in a bit of an unconventional manner.
    std::vector<DragDrop> drag_drop_stack;

    SDL_Texture* example_texture;

    ProgramState(SDL_Renderer* _renderer);

    //This is meant to write the project file for the UFO-Engine project you are making
    void WriteProjectFile(std::string _path);

    //This is meant to load the project file for the UFO-Engine project you are making
    void OpenProjectFile(std::string _path);

    //This opens a folder but will also be used when opening a project
    void OpenFolder(std::string _path);

    bool should_search_working_directory_for_exposed_actor_classes = true;

    void SearchWorkingDirectoryForExposedActorClasses();

    void ImportHeaderFileToProject(std::string _path);

    //This runs after the directory tree is updated
    void Update(); 

    void CleanUp();

};

};