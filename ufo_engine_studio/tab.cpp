#include <filesystem>
#include <string>
#include "../imgui/imgui.h"
#include "tab.h"
#include "editor.h"
#include "file_dialogue.h"
#include "../imgui/imgui_internal.h"

namespace UFOEngineStudio{

Tab::Tab(Editor* _editor) : editor{_editor}{
    id = id_counter++;

    _editor->refresh_entire_project = true;

    name_and_imgui_id = std::string(path+"###Tab"+std::to_string(id));
}

void Tab::Refresh(){
    name = std::filesystem::path(path).filename();
}

void Tab::Update(Editor* _editor, float _delta_time){
    gc.Collect();

    if(ImGui::BeginTabItem(((DetermineIfEdited() ? name : name+"*")+name_and_imgui_id).c_str(), &opened, ImGuiTabItemFlags_None)){

        ImGuiID local_dockspace_id = ImGui::GetID(("###TabDockSpace"+std::to_string(id)).c_str());

        OnMakeDockSpace(local_dockspace_id, _editor);

        ImGui::DockSpace(local_dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoTabBar);

        _editor->active_tab = this;

        OnActive(local_dockspace_id ,_editor, _delta_time);

        //ImGui::EndChildFrame();
        ImGui::EndTabItem();

    }

}

bool Tab::DetermineIfEdited(){
    return false;
}

void Tab::OnMakeDockSpace(ImGuiID _local_dockspace_id, Editor* _program_state){

}

void Tab::OnActive(ImGuiID _local_dockspace_id , Editor* _program_state, float _delta_time){}

void Tab::OnSave(Editor* _program_state){
}

}
