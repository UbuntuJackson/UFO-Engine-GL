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

namespace UFOEngineStudio{

Editor::Editor(){
    OpenFolder("/home/uj/Documents/C++/blitbloot");

}

void Editor::OpenFolder(std::string _path){
    opened_directory = FileNode::ParseFolder(_path);
    opened_directory->file_name = "";

    opened_directory_path = _path;
}

void
Editor::Load(){
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

    if(should_refresh_working_directory) OpenFolder(opened_directory_path);
    should_refresh_working_directory = false;

    ImGui::End();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("Save")){
                if(active_tab) active_tab->OnSave(this);
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
                std::thread t(&BuildAndRunProgram, build_directory);
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
        refresh_entire_project = false;
    }

    gc.Collect();
}

void BuildAndRunProgram(const std::string& _build_directory){
    if(!std::filesystem::exists(_build_directory.c_str())){
        std::filesystem::create_directory(_build_directory.c_str());
    }
    int success = std::system(std::string("cd "+_build_directory+" && cmake .. -DCMAKE_CXX_FLAGS=\"-ggdb\" && make -j8 && gdb OUT").c_str());
    Console::PrintLine("[UFO-Engine Studio] Project Process Success?", success);
}

}
