#include "file_node.h"
#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "directory.h"
#include "file.h"
#include "../file/file.h"
#include <filesystem>
#include <cstdio>
#include "editor.h"

namespace UFOEngineStudio{

void Directory::Update(int _file_index, Directory* _parent,std::string path , Editor* _editor){

    bool folder_opened = ImGui::TreeNodeEx(editing_name ? ("###Directory"+std::to_string(id)).c_str() : (file_name+"###Directory"+path+"/"+file_name).c_str());

    if(editing_name){
        ImGui::SameLine();

        ImGui::InputText(("###EditText"+std::to_string(id)).c_str(),&file_name);

        if((ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsItemHovered()) || ImGui::IsKeyPressed(ImGuiKey_Enter)){
            editing_name = false;
            if(!is_new_directory) std::filesystem::rename(_editor->opened_directory_path + path + "/" + old_file_name, _editor->opened_directory_path + path + "/" + file_name);
            else{
                std::string full_path = _editor->opened_directory_path + path+"/"+file_name;

                std::filesystem::create_directory(full_path);
            }
            is_new_directory = false;

            _editor->should_refresh_working_directory = true;
        }
    }

    //DRAG DROP FEATURE TO BE REWORKED

    FileNode::Update(_file_index, _parent,path, _editor);

    if(ImGui::BeginDragDropTarget()){
        //if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)){
            const ImGuiPayload* payload_data = ImGui::AcceptDragDropPayload("FileDragDrop");
            if(payload_data){
                FileNode* file_node = (FileNode*)(payload_data->Data);

                const std::string this_path = _editor->opened_directory_path + path + "/" + file_name;

                Console::PrintLine(file_node->path_for_drag_drop_payload_use_only);

                std::filesystem::rename(file_node->path_for_drag_drop_payload_use_only, this_path+"/"+file_node->file_name);
                _editor->should_refresh_working_directory = true;
            }
        //}

        ImGui::EndDragDropTarget();
    }

    if(ImGui::BeginPopupContextItem(("Options"+std::to_string(id)).c_str())){
        if(ImGui::MenuItem("Rename")){
            TurnOnEditMode();
        }
        if(ImGui::MenuItem("Delete")){
            std::string full_path = _editor->opened_directory_path + path+"/"+file_name;
            int res = std::remove(full_path.c_str());
            if(res) Console::PrintLine("Directory::Update(): failed to remove directory from at path", full_path.c_str());
            _editor->should_refresh_working_directory = true;

        }
        if(ImGui::MenuItem("New File")){
            file_nodes_to_be_added_at_end_of_frame.push_back(std::make_unique<TreeFile>(true));
            file_nodes_to_be_added_at_end_of_frame.back()->editing_name = true;

        }
        if(ImGui::MenuItem("New Folder")){

            file_nodes_to_be_added_at_end_of_frame.push_back(std::make_unique<Directory>(true));
            file_nodes_to_be_added_at_end_of_frame.back()->file_name = "NewFolder";
            file_nodes_to_be_added_at_end_of_frame.back()->TurnOnEditMode();
        }
        ImGui::EndPopup();
    }

    if(folder_opened){

        for(int index = 0; index < file_nodes.size(); index++){
            file_nodes[index]->Update(index,this,(_parent ? path+"/" : "")+file_name,_editor);
        }

        ImGui::TreePop();

    }

}

}
