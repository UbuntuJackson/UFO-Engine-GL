#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "console.h"
#include "file_node.h"
#include "file.h"
#include "directory.h"
#include "editor.h"
#include <exception>
#include <filesystem>
#include <memory>
#include "file_dialogue.h"
#include "../utils/file_utils.h"
#include "level_editor_tab.h"
#include "text_editor_tab.h"
#include "error_dialogue.h"
#include "ufo_macros.h"

namespace UFOEngineStudio{

    void TreeFile::Update(int _file_index, Directory* _parent,std::string path , Editor* _editor){

        if(editing_name){
            ImGui::InputText(("###EditText"+std::to_string(id)).c_str(),&file_name);

            //This is just duplicate code from this file to make the actual file you just input
            if(!ImGui::IsItemClicked() && (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsKeyPressed(ImGuiKey_Enter))){
                editing_name = false;

                if(is_new_file){
                    try{
                        std::string save_path = _editor->opened_directory_path + path+"/"+file_name;

                        ufo::FileSystem::Write(save_path, "");
                    }
                    catch(const std::exception& _error){
                        Console::PrintLine("[UFO Engine Studio]", _error.what());
                    }

                    is_new_file = false;
                }
                else{
                    try{
                        ufo::FileSystem::Rename(_editor->opened_directory_path + path + "/" + old_file_name, _editor->opened_directory_path + path + "/" + file_name);

                    }
                    catch(const std::exception& _error){
                        Console::PrintLine("[UFO Engine Studio]", _error.what());
                    }
                }
                _editor->refresh_entire_project = true;
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
                            _editor->refresh_entire_project = true;
                        }
                    }
                //}

                ImGui::EndDragDropTarget();
            }

        }

        if(ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered()){

            bool is_valid_file_extension = false;

            //If the file is a level file
            if(ufo::FileSystem::HasExtension(path+"/"+file_name, "ason")){
                ufo::gc::JsonMap* level_json = ufo::gc::JsonRead(&(_editor->gc), _editor->opened_directory_path+path+"/"+file_name);

                //add some sort of LevelOK flag here or something
                ufo::Level* level = _editor->AddActorUniquePtr(
                        std::move(_editor->engine->actor_generator->JsonToActorTree(&(_editor->gc), level_json))
                    )->DynamicCast<ufo::Level>();

                if(!level){
                    _editor->error_dialogue = std::make_unique<ErrorDialogueFailedToOpenFile>(_editor->opened_directory_path+"/"+path+"/"+file_name);
                    Console::PrintLine("[UFO-Engine Studio] Could not convert actor to class Level");
                }
                else{

                    auto level_editor_tab = std::make_unique<LevelEditorTab>(_editor->engine,_editor);
                    level_editor_tab->this_level = level;
                    level_editor_tab->path = _editor->opened_directory_path+path+"/"+file_name;
                    level_editor_tab->Initialise();

                    _editor->tabs.push_back(std::move(level_editor_tab));
                    _editor->refresh_entire_project = true;
                }

                is_valid_file_extension = true;

            }

            if(ufo::FileSystem::HasExtension(path+"/"+file_name, "cpp") ||
                ufo::FileSystem::HasExtension(path+"/"+file_name, "h") ||
                ufo::FileSystem::HasExtension(path+"/"+file_name, "txt") ||
                ufo::FileSystem::HasExtension(path+"/"+file_name, "json") ||
                ufo::FileSystem::HasExtension(path+"/"+file_name, "cfg")
            ){
                try{
                    auto text_editor_tab = std::make_unique<TextEditorTab>("", "" ,_editor);

                    text_editor_tab->text = ufo::FileSystem::Read(_editor->opened_directory_path+path+"/"+file_name);
                    text_editor_tab->path = _editor->opened_directory_path+path+"/"+file_name;

                    _editor->tabs.push_back(std::move(text_editor_tab));
                    _editor->refresh_entire_project = true;
                } catch(const std::exception& _error){
                    Console::PrintLine("[UFO-Engine Studio]", _error.what());
                }

                is_valid_file_extension = true;
            }
            if(!is_valid_file_extension){
                _editor->error_dialogue = std::make_unique<ErrorDialogueText>(std::string(__UFO_PRETTY_FUNCTION__)+" Error, unknown file type: "+_editor->opened_directory_path+path+"/"+file_name);
            }

        }

        if(!ImGui::IsItemClicked() && (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsKeyPressed(ImGuiKey_Enter))){
            editing_name = false;

            if(is_new_file){
                ufo::FileSystem::Write(_editor->opened_directory_path + path+"/"+file_name,"");

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
                _editor->refresh_entire_project = true;

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

            if(ufo::FileSystem::HasExtension(file_name, "json")){
                if(ImGui::MenuItem("Convert to .ason")){
                    std::system(
                        (std::string("cd ../UFO-Engine/tiled_map_conversion_tool && python3 tiled_map_conversion_tool.py ")
                            + "\"" +_editor->opened_directory_path+"\" \"" +path+"/"+file_name+"\"").c_str()
                    );
                }
            }

            ImGui::EndPopup();
        }

        FileNode::Update(_file_index, _parent,path, _editor);

    }

}
