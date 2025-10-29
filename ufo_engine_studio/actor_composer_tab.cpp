/*

    This class is currently unused

*/

#include <memory>
#include <SDL3/SDL.h>
#include "../imgui/imgui.h"
#include "tab.h"
#include "actor_node.h"
#include "program_state.h"
#include "actor_composer_tab.h"
#include "program_state.h"
#include "main_menu_bar.h"
#include "dock_utils.h"

namespace UFOEngineStudio{

ActorComposerTab::ActorComposerTab(ProgramState* _program_state){
    actor = std::make_unique<ActorNode>();
}

void ActorComposerTab::ConvertJsonToCPP(std::string& _header_file , std::vector<std::string>& _used_actor_classes, JsonDictionary* _parent_json, ProgramState* _program_state){

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

        _header_file += ("    auto " + child_identifier + " = " + parent_name + "->AddChild<"+child_type+">("+arg_string+");\n");
        
        ConvertJsonToCPP(_header_file, _used_actor_classes, &json->AsDictionary(), _program_state);
    }

}

void ActorComposerTab::OnActive(ImGuiID _local_dockspace_id, ProgramState* _program_state){

    ImGui::Begin("Actor Composition");

    actor->Update(0, nullptr, nullptr, "", _program_state);
    actor->DeleteActorNodesMarkedForDeletion();
    actor->AddActorNodesRecursive();

    ImGui::End();

    ImGui::Begin("Actor Property Browser");

    if(ImGui::Button("Convert to C++")){
        JsonDictionary json = actor->WriteToJson();

        std::string header_file = "";
        std::string includes = "";
        includes+= "#pragma once\n";
        includes+= "#include <ufo_maths.h>\n";

        header_file+= "\n";
        header_file+= "namespace Generated{\n";
        header_file+= "\n";
        header_file+= "void BuildComponentTree("+json.Get("type").AsString()+"* _this){\n";
        header_file+= "\n";
        json.Set("name", "_this");

        std::vector<std::string> used_actor_classes_to_include;

        ConvertJsonToCPP(header_file, used_actor_classes_to_include, &json, _program_state);

        header_file+= "\n";
        header_file+= "}\n";
        for(const auto& used_actor_class : used_actor_classes_to_include){
            for(const auto& [index, actor_class] : _program_state->project.actor_classes){
                if(actor_class.header_file != "" && actor_class.name == used_actor_class) includes+= "#include "+actor_class.header_file+"\n";
            }
        }

        header_file+= "\n";
        header_file+= "}\n"; //Closing off namespace Generated{

        header_file = includes + header_file;

        File f;
        f.Insert(header_file);
        f.Write(_program_state->working_directory_path + "/" + actor->name + "_generated.h");

        //Need to reload the working directory.
        _program_state->should_refresh_working_directory = true;
    }

    ImGui::End();

}

void ActorComposerTab::OnMakeDockSpace(ImGuiID _local_dockspace_id, ProgramState* _program_state){
    ImGuiDockSpaceSplit(_local_dockspace_id, ImGui::GetWindowSize(), "Actor Composition", "Actor Property Browser", SplitDirections::HORIZONTAL);
}

void ActorComposerTab::OnSave(ProgramState* _program_state){
    if(path != "") actor->WriteToJson().Write(path);
    else{
        const char file_location[] = "/home";

        SDL_ShowSaveFileDialog(&OnNewActorFile , this, _program_state->window, nullptr, 0, file_location);
    }
}

}