#pragma once
#include <level.h>
#include <string>
#include <memory>
#include <vector>
#include "file_node.h"
#include "tab.h"
#include <graphics.h>
#include <engine.h>

namespace UFOEngineStudio{

class Editor : public Level{
public:
    std::string opened_directory_path = "";
    bool should_refresh_working_directory = false;

    std::unique_ptr<FileNode> opened_directory = nullptr;

    std::vector<std::unique_ptr<Tab>> tabs;
    Tab* active_tab = nullptr;

    void OpenFolder(std::string _path);
    Editor();
    void Load();
    void ImportHeaderFileToProject(std::string _path);

    void OnUpdate(float _delta_time);

    void EditorUpdatePhase(float _delta_time){
        /*for(const auto& tab : tabs){
            tab->LevelUpdatePhase(_delta_time);
        }*/
    }

    void EditorDrawPhase(){
        /*for(const auto& tab : tabs){
            tab->LevelDrawPhase(engine->graphics.get());
        }*/
    }

};

}