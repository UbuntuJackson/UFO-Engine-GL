#include <exception>
#include <level.h>
#include <string>
#include <camera.h>
#include <filesystem>
#include "file_node.h"
#include "../imgui/imgui.h"
#include "../ufo_engine_studio/dock_utils.h"
#include "file_node.h"
#include "tab.h"
#include "editor.h"
#include <cstdlib>
#include "../src/engine.h"
#include "level_editor_tab.h"
#include "text_editor_tab.h"
#include "file_dialogue.h"
#include "../file/file_utils.h"

namespace UFOEngineStudio{

Editor::Editor(){

}

void Editor::OpenFolder(std::string _path){
    spawnable_actor_map.clear();

    opened_directory = FileNode::ParseFolder(_path);
    opened_directory->file_name = "";

    opened_directory_path = _path;

    engine->ResetUFOEngineStudio();

    tabs.clear();
    active_tab = nullptr;

    refresh_entire_project = true;
}

void Editor::RefreshFolder(){

    opened_directory = FileNode::ParseFolder(opened_directory_path);
    opened_directory->file_name = "";

}

void
Editor::Load(){
    OpenFolder("/home/uj/Documents/C++/sta_replica");

    engine->asset_manager.LoadTexture("../UFO-Engine/res/actor_icon.png","actor_icon", true);

    Level::Load();

    std::system(std::string(std::string("cd ../UFO-Engine/header_tool && python3 "+header_tool_parser + " ")+std::string("\"")+opened_directory_path+std::string("\"")).c_str());

    PopulateSpawnableActorMapWithBaseObjects();

    ReloadSpawnableActorMap();

}

void Editor::ImportHeaderFileToProject(std::string _path){

}

void Editor::OnUpdate(float _delta_time){

    ImGuiWindowFlags im_gui_window_flags = ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

    //UFOEngineStudio::Tutorial()

    ImGui::Begin("DemoDockspaceForGodsSake", nullptr, im_gui_window_flags);

    ImGuiID dock_space_id = ImGui::GetID("DemoDockspaceForGodsSake");

    UFOEngineStudio::ImGuiDockSpaceSplit(dock_space_id, viewport->Size, "File Tree", "TabBarWindow", UFOEngineStudio::SplitDirections::HORIZONTAL);

    ImGui::DockSpace(dock_space_id, ImVec2(0.0f,0.0f), 0);

    ImGui::End();



    ImGui::Begin("File Tree");

    opened_directory->Update(0, nullptr, "", this);

    opened_directory->AddFileNodesRecursive();

    ImGui::End();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("Open Folder")){
                SDL_ShowOpenFolderDialog(&UFOEngineStudio::OnOpenFolder, this, engine->window, "/home", false);
            }

            if(ImGui::MenuItem("Save")){
                if(active_tab){
                    active_tab->OnSave(this);
                    refresh_entire_project = true;
                }
            }

            if (ImGui::BeginMenu("New File"))
            {
                if(ImGui::MenuItem("Level (.ason)")){
                    auto tab = std::make_unique<LevelEditorTab>(engine,this);
                    tab->Initialise();
                    tabs.push_back(std::move(tab));
                }
                if(ImGui::MenuItem("Textfile (.txt)")){
                    tabs.push_back(std::make_unique<TextEditorTab>("","",this));
                }
                /*if(ImGui::MenuItem("Actor (ufo.h)")){
                    tabs.push_back(std::make_unique<TextEditorTab>("","",this));
                    }*/

                ImGui::EndMenu();
            }


            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Project")){
            if(ImGui::MenuItem("Reload Project")){
                refresh_entire_project = true;
            }

            if(ImGui::MenuItem("Run Project")){
                const std::string build_directory = opened_directory_path+"/build";
                std::thread t(&BuildAndRunProgram, build_directory, opened_directory_path);
                t.detach();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    ImGui::Begin("TabBarWindow");

    ImGui::BeginTabBar("TabBar");

    for(const auto& tab : tabs){
        tab->Update(this, _delta_time);
    }

    ImGui::EndTabBar();

    ImGui::End();

    //UFOEngineStudio::ImGuiDockSpaceSplit(dock_space_id, viewport->Size, "File Tree", "TabBarWindow", UFOEngineStudio::SplitDirections::HORIZONTAL);

    if(refresh_entire_project){
        spawnable_actor_map.clear();
        std::system(std::string(std::string("cd ../UFO-Engine/header_tool && python3 "+header_tool_parser + " ")+std::string("\"")+opened_directory_path+std::string("\"")).c_str());
        PopulateSpawnableActorMapWithBaseObjects();
        ReloadSpawnableActorMap();
        for(const auto& tab : tabs){
            tab->Refresh();
        }
        engine->asset_manager.SaveAssets();
        Console::PrintLine("Refreshed entire project");

        //Refresh entire file tree.
        RefreshFolder();

        refresh_entire_project = false;
    }

    gc.Collect();
}

void BuildAndRunProgram(const std::string& _build_directory, const std::string& _opened_directory_path){
    try{
        std::string cmake_file_path = _opened_directory_path+"/CMakeLists.txt";
        if(!ufo::FileSystem::FileExists(cmake_file_path)){
            std::string cmake_file = ufo::FileSystem::Read("../UFO-Engine/project_templates/CMakeLists.txt");
            ufo::FileSystem::Write(cmake_file_path, cmake_file);
        }
    } catch(const std::exception& _error){
        Console::PrintLine(_error.what());
    }

    try{
        std::string main_file_path= _opened_directory_path+"/main.cpp";

        if(!ufo::FileSystem::FileExists(main_file_path)){
            std::string main_file = ufo::FileSystem::Read("../UFO-Engine/project_templates/main.cpp");
            ufo::FileSystem::Write(main_file_path, main_file);
        }
    } catch(const std::exception& _error){
        Console::PrintLine(_error.what());
    }

    if(ufo::FileSystem::FileExists(_opened_directory_path+"/UFO-Engine-GL")){
        Console::PrintLine("[UFO-Engine Studio] Error, please rename cloned repository to UFO-Engine");
        return;
    }

    if(!ufo::FileSystem::FileExists(_opened_directory_path+"/UFO-Engine")){
        Console::PrintLine("[UFO-Engine Studio] Could not find folder "+_opened_directory_path+"/UFO-Engine");
        return;
    }

    if(!std::filesystem::exists(_build_directory.c_str())){
        std::filesystem::create_directory(_build_directory.c_str());
    }

    //Could build with max available CPU here.
    int success = std::system(std::string("cd "+_build_directory+" && cmake .. -DCMAKE_CXX_FLAGS=\"-ggdb\" && make -j8").c_str());
    Console::PrintLine("[UFO-Engine Studio] Project Process Success?", success);
}

}
