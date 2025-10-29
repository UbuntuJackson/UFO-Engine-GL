#include <level.h>
#include <string>
#include <memory>
#include <camera.h>
#include "file_node.h"
#include "../imgui/imgui.h"
#include "../ufo_engine_studio/dock_utils.h"
#include "file_node.h"
#include "tab.h"
#include "editor.h"
#include "level_editor_tab_gl.h"
#include "controllable_camera.h"

namespace UFOEngineStudio{

void Editor::OpenFolder(std::string _path){
    opened_directory = FileNode::ParseFolder(_path);
    opened_directory->file_name = "";
    
    opened_directory_path = _path;
}

Editor::Editor(){
    OpenFolder("/home/uj/Documents/C++/blitbloot");

}

void
Editor::Load(){

    Level::Load();

    tabs.push_back(std::make_unique<LevelEditorTab>(engine, this));
    tabs.push_back(std::make_unique<LevelEditorTab>(engine, this));
    tabs.push_back(std::make_unique<Tab>());
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

    ImGui::Begin("DemoDockspaceForGodsSake", nullptr, im_gui_window_flags);

    ImGuiID dock_space_id = ImGui::GetID("DockSpaceOutsideTab");

    UFOEngineStudio::ImGuiDockSpaceSplit(dock_space_id, viewport->Size, "File Tree", "TabBarWindow", UFOEngineStudio::SplitDirections::HORIZONTAL);

    ImGui::DockSpace(dock_space_id, ImVec2(0.0f,0.0f), 0);

    ImGui::End();



    ImGui::Begin("File Tree");

    opened_directory->Update(0, nullptr, "", this);

    opened_directory->AddFileNodesRecursive();

    if(should_refresh_working_directory) OpenFolder(opened_directory_path);
    should_refresh_working_directory = false;

    ImGui::End();

    ImGui::Begin("TabBarWindow");

    ImGui::BeginTabBar("TabBar");

    for(const auto& tab : tabs){
        tab->Update(this, _delta_time);
    }

    ImGui::EndTabBar();

    ImGui::End();

    //UFOEngineStudio::ImGuiDockSpaceSplit(dock_space_id, viewport->Size, "File Tree", "TabBarWindow", UFOEngineStudio::SplitDirections::HORIZONTAL);

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("Save")){
                active_tab->OnSave(this);
            }

            

            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }

}

}