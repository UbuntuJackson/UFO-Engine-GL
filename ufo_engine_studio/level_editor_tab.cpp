#include "level_editor_tab.h"
#include <string>
#include <memory>
#include <iostream>
#include <sstream>
#include "../console/console.h"
#include <SDL3/SDL.h>
#include "../json/json_variant.h"
#include "editor.h"
#include "../UFO-Engine-GL/imgui/imgui.h"
#include "../UFO-Engine-GL/imgui/misc/cpp/imgui_stdlib.h"
#include "im_vec.h"
#include "dock_utils.h"
#include "actor_node.h"
#include "main_menu_bar.h"
#include "file.h"

namespace UFOEngineStudio{

LevelEditorTab::LevelEditorTab(Editor* _program_state,std::string _file){
    name = _file.substr(_file.find_last_of("/")+1);
    actor = std::make_unique<ActorNode>();
    actor->name = "LevelRoot";
    _program_state->should_refresh_properties_on_all_nodes = true;
}

void LevelEditorTab::Update(Editor* _program_state){
    Tab::Update(_program_state);

    actor->UpdateExportedAttributes(_program_state);
}

void LevelEditorTab::ConvertActorTreeToCPP(Editor* _program_state){
    JsonDictionary json = actor->WriteToJson();

    std::string top_node_name = json.Get("name").AsString();

    std::string header_file = "";
    std::string includes = "";
    std::string handle_struct = "";
    std::string handle_identifiers = "";
    includes+= "#pragma once\n";
    includes+= "#include <ufo_maths.h>\n";

    header_file+= "\n";

    //Make sure there is a struct to import
    std::string handle_struct_name = "ComponentHandles";
    handle_struct += "struct " + handle_struct_name + "{\n";

    header_file+= "\n";
    header_file+= handle_struct_name + " BuildComponentTree("+json.Get("type").AsString()+"* _this){\n";
    header_file+= "\n";
    json.Set("name", "_this");

    std::vector<std::string> used_actor_classes_to_include;

    ConvertJsonToCPP(handle_identifiers, handle_struct, header_file, used_actor_classes_to_include, &json, _program_state);

    header_file += "    return "+handle_struct_name+"{"+handle_identifiers.substr(0,handle_identifiers.size()-1)+"};\n";

    handle_struct += "};\n";

    header_file+= "\n";
    header_file+= "}\n";
    for(const auto& used_actor_class : used_actor_classes_to_include){
        for(const auto& actor_class : _program_state->project.actor_classes){
            if(actor_class.header_file != "" && actor_class.name == used_actor_class) includes+= "#include "+actor_class.header_file+"\n";
        }
    }

    header_file+= "\n";
    header_file+= "}\n"; //Closing off namespace Generated{

    std::string namespace_start = "namespace Generated"+top_node_name+"{\n";

    header_file = includes + namespace_start + handle_struct + header_file;

    File f;
    f.Insert(header_file);

    std::string inside_of_directory = path.substr(0,path.find_last_of("/"));

    Console::PrintLine("inside directory",inside_of_directory);

    f.Write(inside_of_directory + "/" + actor->name + "_generated.h");

    //Need to reload the working directory.
    _program_state->should_refresh_working_directory = true;
}

void LevelEditorTab::ConvertJsonToCPP(std::string& _handle_identifiers, std::string& _handle_struct, std::string& _header_file , std::vector<std::string>& _used_actor_classes, JsonDictionary* _parent_json, Editor* _program_state){

    std::string parent_name = _parent_json->Get("name").AsString();
    std::string parent_type = _parent_json->Get("type").AsString();

    //Is the include already included?
    auto res = std::find(_used_actor_classes.begin(), _used_actor_classes.end(), parent_type);

    bool include_not_already_included = (res == _used_actor_classes.end());

    //Adds the include
    if(include_not_already_included) _used_actor_classes.push_back(parent_type);

    //Producing the code taht adds the children to the parent.
    for(const auto& json : _parent_json->Get("children").AsArray().Iterable()){
        std::string child_identifier = json->AsDictionary().Get("name").AsString();
        std::string child_type =  json->AsDictionary().Get("type").AsString();
        auto& constructor_properties = json->AsDictionary().Get("constructor_properties").AsArray().Iterable();

        std::string arg_string = "";
        int index = 0;
        for(const auto& property : constructor_properties){
            auto& property_dict = property->AsDictionary();

            if(property_dict.Get("type").AsString() == "int"){
                arg_string += std::to_string(property_dict.Get("value").AsInt());
            }
            else if(property_dict.Get("type").AsString() == "String"){
                arg_string += "\""+ property_dict.Get("value").AsString() + "\"";
            }
            else if(property_dict.Get("type").AsString() == "AssetString"){
                arg_string += "\""+ _program_state->asset_manager.textures.at(property_dict.Get("value").AsString())->alias + "\"";
            }
            else if(property_dict.Get("type").AsString() == "float"){
                arg_string += std::to_string(property_dict.Get("value").AsFloat());
            }
            else if(property_dict.Get("type").AsString() == "Vector2f"){
                arg_string += "Vector2f(";
                arg_string += std::to_string(property_dict.Get("x").AsFloat()) + ",";
                arg_string += std::to_string(property_dict.Get("y").AsFloat());
                arg_string += ")";
            }
            else{
                Console::PrintLine("ActorComposerTab::ConvertJsonToCPP: Unrecognised type:", property_dict.Get("type").AsString());
            }
            if(index != constructor_properties.size()-1) arg_string+= ",";
            index++;
        }

        _header_file += ("    auto " + child_identifier + " = " + parent_name + "->AddActor<"+child_type+">("+arg_string+");\n");

        std::string ordinary_properties_string = "";
        auto& properties = json->AsDictionary().Get("properties").AsArray().Iterable();
        for(const auto& property : properties){
            auto& property_dict = property->AsDictionary();

            if(property_dict.Get("type").AsString() == "int"){
                ordinary_properties_string += child_identifier + "->" +  property_dict.Get("name").AsString() + "=" + std::to_string(property_dict.Get("value").AsInt());
            }
            else if(property_dict.Get("type").AsString() == "String"){
                ordinary_properties_string += child_identifier + "->" +  property_dict.Get("name").AsString() + "=" + "\""+ property_dict.Get("value").AsString() + "\"";
            }
            else if(property_dict.Get("type").AsString() == "AssetString"){
                ordinary_properties_string += child_identifier + "->" +  property_dict.Get("name").AsString() + "=" +"\""+ _program_state->asset_manager.textures.at(property_dict.Get("value").AsString())->alias + "\"";
            }
            else if(property_dict.Get("type").AsString() == "float"){
                ordinary_properties_string += child_identifier + "->" +  property_dict.Get("name").AsString() + "=" +std::to_string(property_dict.Get("value").AsFloat());
            }
            else if(property_dict.Get("type").AsString() == "Vector2f"){
                ordinary_properties_string += child_identifier + "->" +  property_dict.Get("name").AsString() + "=" + "Vector2f(";
                ordinary_properties_string += std::to_string(property_dict.Get("x").AsFloat()) + ",";
                ordinary_properties_string += std::to_string(property_dict.Get("y").AsFloat());
                ordinary_properties_string += ")";
            }
            else{
                Console::PrintLine("ActorComposerTab::ConvertJsonToCPP: Unrecognised type:", property_dict.Get("type").AsString());
            }
            ordinary_properties_string += ";\n";
            
        }
        
        _header_file += ordinary_properties_string;

        //Baked
        std::string exported_properties_string = "";
        auto& exported_properties = json->AsDictionary().Get("exported_properties").AsArray().Iterable();
        for(const auto& property : exported_properties){
            auto& property_dict = property->AsDictionary();

            if(property_dict.Get("type").AsString() == "int"){
                exported_properties_string += child_identifier + "->" +  property_dict.Get("name").AsString() + "=" + std::to_string(property_dict.Get("value").AsInt());
            }
            else if(property_dict.Get("type").AsString() == "String"){
                exported_properties_string += child_identifier + "->" +  property_dict.Get("name").AsString() + "=" + "\""+ property_dict.Get("value").AsString() + "\"";
            }
            else if(property_dict.Get("type").AsString() == "AssetString"){
                exported_properties_string += child_identifier + "->" +  property_dict.Get("name").AsString() + "=" +"\""+ _program_state->asset_manager.textures.at(property_dict.Get("value").AsString())->alias + "\"";
            }
            else if(property_dict.Get("type").AsString() == "float"){
                exported_properties_string += child_identifier + "->" +  property_dict.Get("name").AsString() + "=" +std::to_string(property_dict.Get("value").AsFloat());
            }
            else if(property_dict.Get("type").AsString() == "Vector2f"){
                exported_properties_string += child_identifier + "->" +  property_dict.Get("name").AsString() + "=" + "Vector2f(";
                exported_properties_string += std::to_string(property_dict.Get("x").AsFloat()) + ",";
                exported_properties_string += std::to_string(property_dict.Get("y").AsFloat());
                exported_properties_string += ")";
            }
            else{
                Console::PrintLine("ActorComposerTab::ConvertJsonToCPP: Unrecognised type:", property_dict.Get("type").AsString());
            }
            exported_properties_string += ";\n";
            
        }
        
        _header_file += exported_properties_string;

        std::string animation_args = "";

        if(child_type == "Animation"){
            _header_file+= "    //animation " + child_identifier + "\n";
            if(json->AsDictionary().KeyExists("animation_sprites")){
                int index = 0;
                for(const auto& anim_sprite_json : json->AsDictionary().Get("animation_sprites").AsArray().Iterable()){

                    auto& property_dict = anim_sprite_json->AsDictionary();

                    if(property_dict.Get("type").AsString() == "int"){
                        animation_args += std::to_string(property_dict.Get("value").AsInt());
                    }
                    else if(property_dict.Get("type").AsString() == "String"){
                        animation_args += "\""+ property_dict.Get("value").AsString() + "\"";
                    }
                    else if(property_dict.Get("type").AsString() == "AssetString"){
                        animation_args += "\""+ _program_state->asset_manager.textures.at(property_dict.Get("value").AsString())->alias + "\"";
                    }
                    else if(property_dict.Get("type").AsString() == "float"){
                        animation_args += std::to_string(property_dict.Get("value").AsFloat());
                    }
                    else if(property_dict.Get("type").AsString() == "Vector2f"){
                        animation_args += "Vector2f(";
                        animation_args += std::to_string(property_dict.Get("x").AsFloat()) + ",";
                        animation_args += std::to_string(property_dict.Get("y").AsFloat());
                        animation_args += ")";
                    }
                    else{
                        Console::PrintLine("ActorComposerTab::ConvertJsonToCPP: Unrecognised type:", property_dict.Get("type").AsString());
                    }
                    if(index != constructor_properties.size()-1) animation_args+= ",";
                    index++;
                }

                _header_file += child_identifier+"->AddAnimationState(AnimationSpriteReference(" + animation_args + "));\n";
            }
        }

        _handle_struct += child_type+"* "+child_identifier+"=nullptr;\n";
        _handle_identifiers += "\n        " + child_identifier+",";

        ConvertJsonToCPP(_handle_identifiers, _handle_struct, _header_file, _used_actor_classes, &json->AsDictionary(), _program_state);
    }

}

void LevelEditorTab::OnActive(ImGuiID _local_dockspace_id , Editor* _program_state){

    ImGui::Begin("LevelContentBrowser");

    ImGuiID level_content_browser_dock = ImGui::GetID("LevelContentBrowserDock");
    ImGuiDockSpaceSplit(level_content_browser_dock, ImGui::GetWindowSize(), "Instances", "Classes", SplitDirections::VERTICAL);
    ImGui::DockSpace(level_content_browser_dock, ImVec2(0.0f, 0.0f));

    ImGui::End();

    ImGui::Begin("Classes");

    int unique_id_counter = 0;

    TreeFile* header_file_with_class_to_add = nullptr;

    for(const auto& actor_class : _program_state->project.actor_classes){
        
        bool pressed = ImGui::ImageButton(("###ClassIconUniqueEntry"+std::to_string(unique_id_counter)).c_str(), (ImTextureID)(intptr_t)_program_state->example_texture, ImVec2(_program_state->example_texture->w, _program_state->example_texture->h));

        if(ImGui::BeginDragDropTarget()){
            if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)){

                const ImGuiPayload* drag_drop_from_file_menu = ImGui::GetDragDropPayload();

                if("FROM_FILE" == std::string(drag_drop_from_file_menu->DataType)){
            
                    FileNode* file = (FileNode*)drag_drop_from_file_menu->Data;
            
                    if(file->file_name.substr(file->file_name.find_last_of(".")) == ".h" ||
                        file->file_name.substr(file->file_name.find_last_of(".")) == ".hpp")
                    {
                        //adding while iterating is not cool!
                        header_file_with_class_to_add = (TreeFile*)file;
                    }
                }
            }
            
            ImGui::EndDragDropTarget();
        }

        if(!actor_variant_config_open && pressed){
            actor_variant_config_open = true;
        }

        ImGui::SameLine();
        ImGui::Text((actor_class.name).c_str());

        unique_id_counter++;
    }

    //Need to delay the importing of header file to avoid adding an actor_class while iterator though actor_classes
    if(header_file_with_class_to_add != nullptr){
        _program_state->ImportHeaderFileToProject(_program_state->working_directory_path + header_file_with_class_to_add->path_for_drag_drop_payload_use_only);
    }

    ImGui::End();

    ImGui::Begin("Instances");

    actor->Update(0 ,this,nullptr, "" , _program_state);
    actor->DeleteActorNodesMarkedForDeletion();
    actor->AddActorNodesRecursive();
    if(something_was_selected_this_frame) actor->SearchAndResetSelected();
    something_was_selected_this_frame = false;

    ImGui::End();

    /*ImGui::Begin("InstanceBrowser");

    if(!actor->UpdatePropertyBrowserIfSelected()){
        ImGuiID local_dockspace_id = ImGui::GetID("TemporaryNewDock");
        ImGuiDockSpaceFill(local_dockspace_id, ImGui::GetWindowSize(), "Instances", false);
        ImGui::DockSpace(local_dockspace_id, ImVec2(0.0f, 0.0f));
    }
    else{
        ImGuiID local_dockspace_id = ImGui::GetID("TemporaryNewDock");
        ImGuiDockSpaceSplit(local_dockspace_id, ImGui::GetWindowSize(), ("Instance Properties " + name + "###Instance Properties").c_str(), "Instances", SplitDirections::VERTICAL, false);
        ImGui::DockSpace(local_dockspace_id, ImVec2(0.0f, 0.0f));
    }

    ImGui::End();*/

    ImGui::Text(name.c_str());

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar;

    ImGui::Begin("Level Grid", nullptr, window_flags);

    ImGui::BeginChild("Drawing Surface",ImVec2(2000.0f, 1000.0f));

    actor->DrawInLevel(_program_state, nullptr, Vector2f(0.0f, 0.0f));

    ImGui::EndChild();

    ImGui::End();
    
}

void LevelEditorTab::OnMakeDockSpace(ImGuiID _local_dockspace_id, Editor* _program_state){
    ImGuiDockSpaceSplit(_local_dockspace_id, ImGui::GetWindowSize(), "Level Grid", "LevelContentBrowser", SplitDirections::HORIZONTAL);
}

void LevelEditorTab::OnSave(Editor* _program_state){
    ConvertActorTreeToCPP(_program_state);

    if(path != "") actor->WriteToJson().Write(path);
    else{
        const char* file_location = _program_state->working_directory_path.c_str();

        SDL_ShowSaveFileDialog(&OnNewActorFile , this, _program_state->window, nullptr, 0, file_location);
    }
    _program_state->should_refresh_working_directory = true;
}

}