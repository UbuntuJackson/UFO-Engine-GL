#include <string>
#include <memory>
#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "actor_node.h"
#include "program_state.h"
#include "../console/console.h"
#include "../json/json_variant.h"
#include "level_editor_tab.h"
#include "property.h"
#include "editor_objects/editor_object.h"
#include "editor_objects/sprite_reference_editor_object.h"
#include "editor_objects/animation_editor_object.h"
#include "dock_utils.h"
#include "editor_objects/tile_map_editor_object.h"

namespace UFOEngineStudio{

ActorNode::ActorNode(){
    id = id_incrementation_counter++;
    name = "Actor_"+std::to_string(id);

    editor_object = std::make_unique<ActorEditorObject>();
}

void ActorNode::UpdateExportedAttributes(ProgramState* _program){

    //for(const auto& property : editor_object->properties) property->InactiveUpdate(_program);
    //for(const auto& property : editor_object->exported_properties) property->InactiveUpdate(_program);
    //for(const auto& property : editor_object->constructor_properties) property->InactiveUpdate(_program);

    if(_program->should_refresh_properties_on_all_nodes){
        editor_object->UpdateExportedAttributes(_program);

    }

    for(const auto& actor_node : actor_nodes){
        actor_node->UpdateExportedAttributes(_program);
    }
}

void ActorNode::Update(int _file_index, LevelEditorTab* _level_editor_tab, ActorNode* _parent, std::string path , ProgramState* _program){
    bool folder_opened = ImGui::TreeNodeEx(editing_name ? ("###Directory"+std::to_string(id)).c_str() : (name+" ("+actor_type+")"+"###Directory"+std::to_string(id)).c_str());

    if(editing_name){
        ImGui::SameLine();
        
        ImGui::InputText(("###EditText"+std::to_string(id)).c_str(),&name);

        if(ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsItemHovered()){
            editing_name = false;
        }

        if(ImGui::IsKeyPressed(ImGuiKey_Enter)){
            editing_name = false;
        }
    }

    if(editing_type){
        ImGui::Begin("Change ActorNode Type");
        for(const auto& actor_class : _program->project.actor_classes){
            if(ImGui::Button(actor_class.name.c_str())){
                actor_type = actor_class.name;

                //Generate a new object here, only instantiates an ActorEditorObject for now.s
                editor_object = CreateEditorObjectWithTypeFromString(actor_class.name);

                editor_object->exported_properties.clear();

                if(actor_class.is_custom_class){
                    for(const auto& property : actor_class.exported_properties){
                        editor_object->exported_properties.push_back(property->Copy());
                    }

                    editor_object->class_name = actor_class.name;
                }

                editing_type = false;
            }
        }

        ImGui::End();
    }

    if(add_actor_node_dialogue_open){
        ImGui::Begin("Add ActorNode");

        for(const auto& actor_class : _program->project.actor_classes){
            if(ImGui::Button(actor_class.name.c_str())){
                auto new_actor_node = std::make_unique<ActorNode>();
                new_actor_node->actor_type = actor_class.name;
                
                //Change type of the node
                new_actor_node->editor_object = CreateEditorObjectWithTypeFromString(actor_class.name);

                new_actor_node->editing_type = false;

                if(actor_class.is_custom_class){
                    for(const auto& property : actor_class.exported_properties){
                        new_actor_node->editor_object->exported_properties.push_back(property->Copy());
                    }

                    new_actor_node->editor_object->class_name = actor_class.name;
                }

                actor_nodes_to_be_added_at_end_of_frame.push_back(std::move(new_actor_node));
                add_actor_node_dialogue_open = false;

            }
        }

        ImGui::End();
    }

    if(show_properties){

        ImGui::Begin(("Instance Properties " + name + "###Instance Properties").c_str());

        editor_object->PropertyUpdate(_program);

        ImGui::End();
    }

    if(ImGui::IsItemClicked()){
        was_selected_this_frame = true;
        _level_editor_tab->something_was_selected_this_frame = true;

        //_program->drag_drop_stack.push_back(DragDrop{this, _parent});
    }

    //Unconventional use of BeginDrawDropSource and SetDragDropPayload
    if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)){
        ImGui::SetDragDropPayload("CONTENT_BROWSER_DATA", this, sizeof(this));

        ImGui::EndDragDropSource();
    }

    //This is in case file is dropped on folder
    if(ImGui::BeginDragDropTarget()){

        if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)){
            
            //_program->drag_drop_stack.back().move_to_folder = this;
            //_program->drag_drop_stack.push_back();
        }
        //ImGui::AcceptDragDropPayload("CONTENT_BROWSER_DATA");

        ImGui::EndDragDropTarget();
    }

    if(ImGui::BeginPopupContextItem(("Options"+std::to_string(id)).c_str())){
        if(ImGui::MenuItem("Rename")){
            editing_name = true;
        }
        if(ImGui::MenuItem("Delete")){
            to_be_deleted = true;
        }
        if(ImGui::MenuItem("New Actor/Component")){
            //actor_nodes_to_be_added_at_end_of_frame.push_back(std::make_unique<ActorNode>());
            //actor_nodes_to_be_added_at_end_of_frame.back()->editing_name = true;

            add_actor_node_dialogue_open = true;
            
        }

        if(ImGui::MenuItem("Change ActorNode Type")){
            //actor_nodes_to_be_added_at_end_of_frame.push_back(std::make_unique<ActorNode>());
            //actor_nodes_to_be_added_at_end_of_frame.back()->editing_name = true;

            editing_type = true;
            
        }

        ImGui::EndPopup();
    }

    if(folder_opened){

        for(int index = 0; index < actor_nodes.size(); index++){
            actor_nodes[index]->Update(index,_level_editor_tab,this,(_parent ? path+"/" : "")+name,_program);
        }

        ImGui::TreePop();
        
    }
}

void ActorNode::AddActorNodesRecursive(){
    for(auto&& i : actor_nodes_to_be_added_at_end_of_frame){
        actor_nodes.push_back(std::move(i));
    }
    actor_nodes_to_be_added_at_end_of_frame.clear();

    for(auto&& i : actor_nodes){
        i->AddActorNodesRecursive();
    }
}

void ActorNode::DeleteActorNodesMarkedForDeletion(){

    for(int i = actor_nodes.size()-1; i >= 0; i--){
        if(actor_nodes[i]->to_be_deleted) actor_nodes.erase(actor_nodes.begin() + i);
    }

    for(int i = actor_nodes.size()-1; i >= 0; i--){
        actor_nodes[i]->DeleteActorNodesMarkedForDeletion();
    }
}

JsonDictionary ActorNode::WriteToJson(){

    JsonDictionary this_actor = JsonDictionary();

    this_actor.Set("name", name);
    this_actor.Set("type", actor_type);
    this_actor.Set("children", JsonArray());
    this_actor.Set("constructor_properties", JsonArray()); //This is going in editor_object
    this_actor.Set("properties", JsonArray()); //This is going in editor_object
    this_actor.Set("exported_properties", JsonArray()); //This is going in editor_object

    this_actor.Set("editor_object_class_name", editor_object->class_name);

    for(const auto& child : actor_nodes){
        this_actor.Get("children").AsArray().Push(child->WriteToJson());
    }

    for(const auto& property : editor_object->constructor_properties){ //This is going in editor_object
        this_actor.Get("constructor_properties").AsArray().Push(property->AsJson());
    }

    for(const auto& property : editor_object->properties){ //This is going in editor_object
        this_actor.Get("properties").AsArray().Push(property->AsJson());
    }

    for(const auto& property : editor_object->exported_properties){ //This is going in editor_object
        this_actor.Get("exported_properties").AsArray().Push(property->AsJson());
    }

    return this_actor;
}

/*std::unique_ptr<ActorNode> ActorNode::ReadFromJsonTypeDiverse(ProgramState* _program_state, JsonDictionary* _json){
    std::unique_ptr<ActorNode> actor_node;

    std::string local_name = _json->Get("name").AsString();
    std::string local_actor_type = _json->Get("type").AsString();

    actor_node = _program_state->ActorNodeFactoryTypeDiverse(local_actor_type);
    ActorNode::SetAttributesFromJsonTypeDiverse(_json, actor_node.get());
    actor_node->Initialise();
}

void ActorNode::SetAttributesFromJsonTypeDiverse(JsonDictionary* _json, ActorNode* _instance){

}*/

std::unique_ptr<ActorEditorObject> ActorNode::CreateEditorObjectWithTypeFromString(std::string _actor_type){

    if(_actor_type == "Sprite"){
        return std::make_unique<SpriteReferenceEditorObject>();
    }

    if(_actor_type == "Animation"){
        return std::make_unique<AnimationEditorObject>();
    }

    if(_actor_type == "TileMap"){
        return std::make_unique<TileMapEditorObject>();
    }

    //Make it an ActorEditorObject by default, ie Actor
    return std::make_unique<ActorEditorObject>();

}

void ActorNode::ReadFromJson(JsonDictionary* _json){
    name = _json->Get("name").AsString();
    actor_type = _json->Get("type").AsString();

    editor_object = CreateEditorObjectWithTypeFromString(actor_type);

    editor_object->SetPropertiesFromJson(_json);
    /*if(actor_type == "Actor"){
        editor_object = std::make_unique<ActorEditorObject>();
        editor_object->SetPropertiesFromJson(_json);
    }*/

    editor_object->class_name = _json->Get("editor_object_class_name").AsString();

    for(const auto& child : _json->Get("children").AsArray().Iterable()){
        auto u_child = std::make_unique<ActorNode>();
        u_child->ReadFromJson(&child->AsDictionary());
        actor_nodes.push_back(std::move(u_child));
    }

}
    
}