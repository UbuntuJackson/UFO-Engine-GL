#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "file_node.h"
#include "file.h"
#include "directory.h"
#include "../file/file.h"
#include "editor.h"
#include <filesystem>
#include <memory>
#include "file_utilities.h"
#include "level_editor_tab.h"

namespace UFOEngineStudio{

    void TreeFile::Update(int _file_index, Directory* _parent,std::string path , Editor* _editor){

        if(editing_name){
            ImGui::InputText(("###EditText"+std::to_string(id)).c_str(),&file_name);

            //This is just duplicate code from this file to make the actual file you just input
            if(!ImGui::IsItemClicked() && (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsKeyPressed(ImGuiKey_Enter))){
                editing_name = false;

                if(is_new_file){
                    File f = File::New();
                    f.Insert("");
                    f.Write(_editor->opened_directory_path + path+"/"+file_name);

                    is_new_file = false;
                }

                _editor->should_refresh_working_directory = true;
            }
        }
        else{

            ImGui::Text("%s",file_name.c_str());

            //THE REST COMMENTED OUT FOR EDITOR REWORK

            if(ImGui::BeginDragDropTarget()){
                //if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)){
                    const ImGuiPayload* payload_data = ImGui::AcceptDragDropPayload("FileDragDrop");
                    if(payload_data){
                        FileNode* file_node = (FileNode*)(payload_data->Data);

                        const std::string this_path = _editor->opened_directory_path + path;

                        Console::PrintLine(file_node->path_for_drag_drop_payload_use_only);
                        if(this_path != file_node->path_for_drag_drop_payload_use_only){
                            std::filesystem::rename(file_node->path_for_drag_drop_payload_use_only, this_path+"/"+file_node->file_name);
                            _editor->should_refresh_working_directory = true;
                        }
                    }
                //}

                ImGui::EndDragDropTarget();
            }

        }

        if(ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered()){
            if(IsExtension(path+"/"+file_name, "ason")){
                ufo::gc::JsonMap* level_json = ufo::gc::JsonRead(&(_editor->gc), _editor->opened_directory_path+path+"/"+file_name);

                auto level = _editor->AddActorUniquePtr(
                        std::move(_editor->engine->actor_generator->JsonToLevelTree(&(_editor->gc), level_json))
                    )->DynamicCast<Level>();

                auto level_editor_tab = std::make_unique<LevelEditorTab>(_editor->engine,_editor);
                level_editor_tab->this_level = level;

                _editor->tabs.push_back(std::move(level_editor_tab));
            }
        }

        if(!ImGui::IsItemClicked() && (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsKeyPressed(ImGuiKey_Enter))){
            editing_name = false;

            if(is_new_file){
                File f = File::New();
                f.Insert("");
                f.Write(_editor->opened_directory_path + path+"/"+file_name);

                is_new_file = false;
            }
        }

        if(ImGui::BeginPopupContextItem(("Options"+std::to_string(id)).c_str())){
            if(ImGui::MenuItem("Rename")){
                TurnOnEditMode();
            }
            if(ImGui::MenuItem("Delete")){
                std::string full_path = _editor->opened_directory_path +"/"+ path+"/"+file_name;
                int res = std::remove(full_path.c_str());
                if(res) Console::PrintLine("TreeFile::Update(): Failture upon trying to remove", full_path.c_str());
                _editor->should_refresh_working_directory = true;

            }
            if(ImGui::MenuItem("New File")){
                _parent->file_nodes_to_be_added_at_end_of_frame.push_back(std::make_unique<TreeFile>(true));
                _parent->file_nodes_to_be_added_at_end_of_frame.back()->editing_name = true;

            }
            if(ImGui::MenuItem("New Folder")){

                std::string full_path = _editor->opened_directory_path + path+"/NewFolder";
                std::filesystem::create_directory(full_path);

                _parent->file_nodes_to_be_added_at_end_of_frame.push_back(std::make_unique<Directory>(true));
                _parent->file_nodes_to_be_added_at_end_of_frame.back()->file_name = "NewFolder";
                _parent->file_nodes_to_be_added_at_end_of_frame.back()->TurnOnEditMode();
            }
            ImGui::EndPopup();
        }

        FileNode::Update(_file_index, _parent,path, _editor);

    }

}
