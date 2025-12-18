#include <filesystem>
#include <memory>
#include <vector>
#include <string>
#include "../utils/console.h"
#include "directory.h"
#include "file.h"
#include "file_node.h"
#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "editor.h"

namespace UFOEngineStudio{

FileNode::FileNode(){
    id = file_id_counter;
    file_id_counter++;
}

std::unique_ptr<FileNode> FileNode::ParseFolder(std::string _path){
    std::unique_ptr<Directory> directory = std::make_unique<Directory>(false);

    directory->file_name = _path.substr(_path.find_last_of("/")+1);

    for(const auto& directory_entry : std::filesystem::directory_iterator{_path}){

        std::error_code ec;
        std::string s_path = std::string(directory_entry.path().string());
        if(std::filesystem::is_directory(directory_entry,ec)){

            directory->file_nodes.push_back(FileNode::ParseFolder(s_path));

        }
        else{

            std::string file_name = s_path.substr(s_path.find_last_of("/")+1);

            auto file = std::make_unique<TreeFile>();

            file->file_name = file_name;

            directory->file_nodes.push_back(std::move(file));

        }
    }

    return std::move(directory);
}

void FileNode::Update(int _file_index, Directory* _parent,std::string path , Editor* _editor){
    if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)){
        path_for_drag_drop_payload_use_only = _editor->opened_directory_path + path + "/" + file_name;
        ImGui::SetDragDropPayload("FileDragDrop", this, sizeof(FileNode));
        ImGui::Text("%s",file_name.c_str());

        ImGui::EndDragDropSource();
    }
}

void FileNode::AddFileNodesRecursive(){
    for(auto&& i : file_nodes_to_be_added_at_end_of_frame){
        file_nodes.push_back(std::move(i));
    }
    file_nodes_to_be_added_at_end_of_frame.clear();

    for(auto&& i : file_nodes){
        i->AddFileNodesRecursive();
    }
}

void FileNode::DeleteFileNodesMarkedForDeletion(){

    for(int i = file_nodes.size()-1; i >= 0; i--){
        if(file_nodes[i]->to_be_deleted){
            file_nodes.erase(file_nodes.begin() + i);
        }
    }

    for(int i = file_nodes.size()-1; i >= 0; i--){
        file_nodes[i]->DeleteFileNodesMarkedForDeletion();
    }
}

void FileNode::Sort(){
    //return;

    auto comp = [](const std::unique_ptr<FileNode>& _a, const std::unique_ptr<FileNode>& _b){
        return _a->file_name < _b->file_name;
    };

    std::sort(file_nodes.begin(), file_nodes.end(),comp);

    for(auto&& i : file_nodes){
        i->Sort();
    }
}

void FileNode::Traverse(){

    for(auto iterator = file_nodes.begin(); iterator != file_nodes.end(); ++iterator){
        (*iterator)->Traverse();
    }
}

void FileNode::SearchForHeaderFiles(Editor* _editor, std::string _path){

    // Don't search through the entire engine.
    if(file_name == "UFO-Engine") return;

    std::string full_path = _path+file_name;

    if(file_name != ""){

        full_path = _path+"/"+file_name;
    }

    if(file_name.find(".") != file_name.npos){
        if(file_name.substr(file_name.find_last_of(".")) == ".h" ||
                            file_name.substr(file_name.find_last_of(".")) == ".hpp")
        {
            Console::PrintLine("Found headerfile:", _editor->opened_directory_path+full_path);
            _editor->ImportHeaderFileToProject(_editor->opened_directory_path+full_path);
        }
    }

    for(const auto& file_node : file_nodes){
        file_node->SearchForHeaderFiles(_editor, full_path);
    }


}

}
