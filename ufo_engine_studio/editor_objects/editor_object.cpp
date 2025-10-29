#include "../vector2f.h"
#include "../property.h"
#include "../../imgui/imgui.h"
#include "../program_state.h"
#include "../im_vec.h"
#include "editor_object.h"

namespace UFOEngineStudio{

ActorEditorObject::ActorEditorObject(){
    local_position_handle = AddConstructorProperty<PropertyVector2f>("local position", 0.0f, 0.0f);
}

void ActorEditorObject::Update(ProgramState* _program_state, ActorEditorObject* _parent_editor_object, Vector2f _parent_position){
    Vector2f global_position = _parent_position + local_position_handle->AsVector2f();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    draw_list->AddRectFilled(ImAdd(ImVec2(global_position.x, global_position.y), ImGui::GetWindowPos()),
        ImAdd(ImVec2(global_position.x+16.0f, global_position.y+16.0f), ImGui::GetWindowPos()),ImColor(255,0,0,255));
}

void ActorEditorObject::PropertyUpdate(ProgramState* _program){
    ImGui::Text("-- Constructor Arguments --");

    for(const auto& property : constructor_properties){
        property->Update(_program);
    }

    ImGui::Text("-- General Properties --");

    for(const auto& property : properties){
        property->Update(_program);
    }

    ImGui::Text("-- Exported Properties --");

    for(const auto& property : exported_properties){
        property->Update(_program);
    }
}

void ActorEditorObject::UpdateExportedAttributes(ProgramState* _program){

    Console::PrintLine("ActorEditorObject::UpdateExportedAttributes");

    //Save in editor object

    for(const auto& actor_class : _program->project.actor_classes){
        if(actor_class.name == class_name){
            
            //If stays false, then remains false
            for(auto&& pre_existing_property : exported_properties){
                pre_existing_property->stays_in_vector = false;
            }

            for(const auto& property : actor_class.exported_properties){

                bool already_exists = false;
                for(auto&& pre_existing_property : exported_properties){
                    if(pre_existing_property->name == property->name){
                        pre_existing_property->stays_in_vector = true;
                        already_exists = true;
                    }
                }

                if(!already_exists) exported_properties.push_back(property->Copy());
            }
            break;
        }
    }

    for(int i = exported_properties.size()-1; i >= 0; i--){
        
        if(!exported_properties[i]->stays_in_vector) exported_properties.erase(exported_properties.begin()+i);
    }

}

void ActorEditorObject::SetPropertiesFromJson(JsonDictionary* _json){
    properties.clear();
    constructor_properties.clear();

    //This goes in editor_object
    for(const auto& property : _json->Get("constructor_properties").AsArray().Iterable()){
        std::string local_type = property->AsDictionary().Get("type").AsString();
        std::string local_name = property->AsDictionary().Get("name").AsString();

        if(local_type == "int"){
            
            int local_value = property->AsDictionary().Get("value").AsInt();
            OnSetupProperty(AddConstructorProperty<PropertyInt>(local_name, local_value));
            
        }
        else if(local_type == "String"){
            
            std::string local_value = property->AsDictionary().Get("value").AsString();
            OnSetupProperty(AddConstructorProperty<PropertyString>(local_name, local_value));

        }
        else if(local_type == "AssetString"){
            
            std::string local_value = property->AsDictionary().Get("value").AsString();
            OnSetupProperty(AddConstructorProperty<PropertyAssetString>(local_name, local_value));

        }
        else if(local_type == "float"){

            float local_value = property->AsDictionary().Get("value").AsFloat();
            OnSetupProperty(AddConstructorProperty<PropertyFloat>(local_name, local_value));
            
        }
        else if(local_type == "Vector2f"){
            float local_value_x = property->AsDictionary().Get("x").AsFloat();
            float local_value_y = property->AsDictionary().Get("y").AsFloat();
            OnSetupProperty(AddConstructorProperty<PropertyVector2f>(local_name, local_value_x, local_value_y));
            
        }
        else{
            Console::PrintLine("ActorNode::ReadFromJson:","Unknown type", local_type);
        }

        
    }

    //This goes in editor_object
    for(const auto& property : _json->Get("properties").AsArray().Iterable()){
        std::string local_type = property->AsDictionary().Get("type").AsString();
        std::string local_name = property->AsDictionary().Get("name").AsString();

        if(local_type == "int"){
            
            int local_value = property->AsDictionary().Get("value").AsInt();
            OnSetupProperty(AddProperty<PropertyInt>(local_name, local_value));
            
        }
        else if(local_type == "String"){
            
            std::string local_value = property->AsDictionary().Get("value").AsString();
            OnSetupProperty(AddProperty<PropertyString>(local_name, local_value));

        }
        else if(local_type == "float"){

            float local_value = property->AsDictionary().Get("value").AsFloat();
            OnSetupProperty(AddProperty<PropertyFloat>(local_name, local_value));
            
        }
        else if(local_type == "Vector2f"){
            float local_value_x = property->AsDictionary().Get("x").AsFloat();
            float local_value_y = property->AsDictionary().Get("y").AsFloat();
            OnSetupProperty(AddProperty<PropertyVector2f>(local_name, local_value_x, local_value_y));
            
        }
        else{
            Console::PrintLine("ActorNode::ReadFromJson:","Unknown type", local_type);
        }
        
    }

    //This goes in editor_object
    for(const auto& property : _json->Get("exported_properties").AsArray().Iterable()){
        std::string local_type = property->AsDictionary().Get("type").AsString();
        std::string local_name = property->AsDictionary().Get("name").AsString();

        if(local_type == "int"){
            
            int local_value = property->AsDictionary().Get("value").AsInt();
            AddExportedProperty<PropertyInt>(local_name, local_value);
            
        }
        else if(local_type == "String"){
            
            std::string local_value = property->AsDictionary().Get("value").AsString();
            AddExportedProperty<PropertyString>(local_name, local_value);

        }
        else if(local_type == "float"){

            float local_value = property->AsDictionary().Get("value").AsFloat();
            AddExportedProperty<PropertyFloat>(local_name, local_value);
            
        }
        else if(local_type == "Vector2f"){
            float local_value_x = property->AsDictionary().Get("x").AsFloat();
            float local_value_y = property->AsDictionary().Get("y").AsFloat();
            AddExportedProperty<PropertyVector2f>(local_name, local_value_x, local_value_y);
            
        }
        else{
            Console::PrintLine("ActorNode::ReadFromJson:","Unknown type", local_type);
        }
        
    }
}

}