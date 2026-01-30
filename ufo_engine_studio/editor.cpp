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
#include "../imgui/imgui_internal.h"

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

    if(ufo::FileSystem::FileExists(opened_directory_path+"/settings.json")){
        auto j_settings = ufo::gc::JsonRead(&gc, opened_directory_path+"/settings.json");
        try{
             v_sync = (bool)j_settings->map["vsync"]->AsFloat();
             multi_player = (bool)j_settings->map["multi_player"]->AsFloat();
             game_width = (int)j_settings->map["game_width"]->AsFloat();
             game_height = (int)j_settings->map["game_height"]->AsFloat();
        }catch(const std::exception& _error){
            Console::PrintLine("[UFO-Engine Studio] Editor: Somehow failed to write property vsync");
        }
        j_settings->Write(opened_directory_path+"/settings.json");
    }
    else{
        auto j_settings = gc.New<ufo::gc::JsonMap>();
        try{
            j_settings->map["vsync"] = gc.New<ufo::gc::JsonNumber>(int(v_sync));
            j_settings->map["game_width"] = gc.New<ufo::gc::JsonNumber>(int(game_width));
            j_settings->map["game_height"] = gc.New<ufo::gc::JsonNumber>(int(game_height));
            j_settings->map["multi_player"] = gc.New<ufo::gc::JsonNumber>(int(multi_player));
        }catch(const std::exception& _error){
            Console::PrintLine("[UFO-Engine Studio] Editor: Somehow failed to write property vsync");
        }
        j_settings->Write(opened_directory_path+"/settings.json");
    }

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

    ImGui::DockSpace(dock_space_id, ImVec2(0.0f,0.0f), ImGuiDockNodeFlags_NoTabBar);

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

            if(ImGui::MenuItem("Compile Game")){
                refresh_entire_project = true;
                will_compile_game = true;
            }

            if(ImGui::MenuItem("Debug Game")){
                const std::string build_directory = opened_directory_path+"/build";
                std::thread t(&DebugGame, build_directory, opened_directory_path);
                t.detach();
            }

            if(ImGui::MenuItem("Run Game")){
                const std::string build_directory = opened_directory_path+"/build";
                std::thread t(&RunGame, build_directory, opened_directory_path);
                t.detach();
            }

            if(ImGui::MenuItem("Settings")){
                project_settings_open = true;
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

    if(project_settings_open){
        ImGui::Begin("Game Settings", &project_settings_open);

        ImGui::Checkbox("Vsync###EditorVsync On", &v_sync);
        ImGui::Checkbox("MultiPlayer###Multiplayer On", &multi_player);

        ImGui::InputInt("Window Width", &game_width);
        ImGui::InputInt("Window Height", &game_height);

        ImGui::Text("Execution mode");

        if(ImGui::Button("Apply & Save")){
            if(ufo::FileSystem::FileExists(opened_directory_path+"/settings.json")){
                auto j_settings = ufo::gc::JsonRead(&gc, opened_directory_path+"/settings.json");
                try{
                    j_settings->map["vsync"] = gc.New<ufo::gc::JsonNumber>(int(v_sync));
                    j_settings->map["game_width"] = gc.New<ufo::gc::JsonNumber>(int(game_width));
                    j_settings->map["game_height"] = gc.New<ufo::gc::JsonNumber>(int(game_height));
                    j_settings->map["multi_player"] = gc.New<ufo::gc::JsonNumber>(int(multi_player));
                }catch(const std::exception& _error){
                    Console::PrintLine("[UFO-Engine Studio] Editor: Somehow failed to write property vsync");
                }
                j_settings->Write(opened_directory_path+"/settings.json");
            }
            else{
                auto j_settings = gc.New<ufo::gc::JsonMap>();
                try{
                    j_settings->map["vsync"] = gc.New<ufo::gc::JsonNumber>(int(v_sync));
                    j_settings->map["game_width"] = gc.New<ufo::gc::JsonNumber>(int(game_width));
                    j_settings->map["game_height"] = gc.New<ufo::gc::JsonNumber>(int(game_height));
                    j_settings->map["multi_player"] = gc.New<ufo::gc::JsonNumber>(int(multi_player));
                }catch(const std::exception& _error){
                    Console::PrintLine("[UFO-Engine Studio] Editor: Somehow failed to write property vsync");
                }
                j_settings->Write(opened_directory_path+"/settings.json");
            }
        }

        ImGui::End();
    }

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

    if(will_compile_game){
        const std::string build_directory = opened_directory_path+"/build";
        std::thread t(&BuildAndRunProgram, build_directory, opened_directory_path);
        t.detach();
        will_compile_game = false;
    }

    gc.Collect();
}

void BuildAndRunProgram(const std::string& _build_directory, const std::string& _opened_directory_path){
    try{
        const std::string cmake_file_path = _opened_directory_path+"/CMakeLists.txt";
        if(!ufo::FileSystem::FileExists(cmake_file_path)){
            const std::string cmake_file = ufo::FileSystem::Read("../UFO-Engine/project_templates/CMakeLists.txt");
            ufo::FileSystem::Write(cmake_file_path, cmake_file);
        }
    } catch(const std::exception& _error){
        Console::PrintLine(_error.what());
    }

    try{
        const std::string main_file_path = _opened_directory_path+"/main.cpp";

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
    int success = std::system(std::string("cd "+_build_directory+" && gnome-terminal -- bash -c \"cmake .. -DCMAKE_CXX_FLAGS=\"-ggdb\" && make -j8 && echo \"\"Press any key to continue...\"\" && read p\"").c_str());
    Console::PrintLine("[UFO-Engine Studio] Project Process Success?", success);
}

void DebugGame(const std::string& _build_directory, const std::string& _opened_directory_path){
    //Could build with max available CPU here.
    int success = std::system(std::string("cd "+_build_directory+" && gnome-terminal -- bash -c \"gdb OUT && echo \"\"Press any key to continue...\"\" && read p\"").c_str());
    Console::PrintLine("[UFO-Engine Studio] Game Run Success?", success);
}

void RunGame(const std::string& _build_directory, const std::string& _opened_directory_path){
    //Could build with max available CPU here.
    int success = std::system(std::string("cd "+_build_directory+" && gnome-terminal -- bash -c \"./OUT && echo \"\"Press any key to continue...\"\" && read p\"").c_str());
    Console::PrintLine("[UFO-Engine Studio] Game Run Success?", success);
}

}
