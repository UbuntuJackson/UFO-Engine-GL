#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "../json/json_variant.h"
#include "property.h"
#include "program_state.h"
#include "editor_objects/animation_editor_object.h"

namespace UFOEngineStudio{

//Property

Property::Property(std::string _name){
    id = id_incrementation_counter++;
    name = _name;
}
void Property::Update(ProgramState* _program_state){
    
}

void Property::InactiveUpdate(ProgramState* _program_state){

}

JsonDictionary Property::AsJson(){
    
}

//PropertyInt

PropertyInt::PropertyInt(std::string _name, int _value) : Property(_name), value{_value}{}

void PropertyInt::Update(ProgramState* _program_state){
    ImGui::InputInt((name+"##"+std::to_string(id)).c_str(), &value);
}

JsonDictionary PropertyInt::AsJson(){
    JsonDictionary j = JsonDictionary();
    j.Set("name", name);
    j.Set("value", value);
    j.Set("type", "int");
    return j;
}

//PropertyBool
PropertyBool::PropertyBool(std::string _name, bool _value) : Property(_name), checked{_value}{}

void PropertyBool::Update(ProgramState* _program_state){
    ImGui::Checkbox((name+"##"+std::to_string(id)).c_str(), &checked);
}

JsonDictionary PropertyBool::AsJson(){
    JsonDictionary j = JsonDictionary();
    j.Set("name", name);
    j.Set("value", checked);
    j.Set("type", "bool");
    return j;
}

//PropertyString

PropertyString::PropertyString(std::string _name, std::string _text) : Property(_name), text{_text}{}

void PropertyString::Update(ProgramState* _program_state){
    ImGui::InputText((name+"##"+std::to_string(id)).c_str(), &text);
}

JsonDictionary PropertyString::AsJson(){
    JsonDictionary j = JsonDictionary();
    j.Set("name", name);
    j.Set("value", text);
    j.Set("type", "String");
    return j;
}

//PropertyAssetString

PropertyAssetString::PropertyAssetString(std::string _name, std::string _asset_path) : Property(_name), asset_path{_asset_path}{}

void PropertyAssetString::InactiveUpdate(ProgramState* _program_state){
    Console::PrintLine("InactiveUpdate");
    /*if(!_program_state->asset_manager.textures.count(asset_path)){
        for(const auto& [key,asset] : _program_state->asset_manager.textures){
            if(asset->alias == "?") asset_path = key;
        }
    }*/
}

void PropertyAssetString::Update(ProgramState* _program_state){

    if(!_program_state->asset_manager.textures.count(asset_path)){
        for(const auto& [key,asset] : _program_state->asset_manager.textures){
            if(asset->alias == "?") asset_path = key;
        }
    }

    if(ImGui::BeginCombo((name+"##"+std::to_string(id)).c_str(), _program_state->asset_manager.textures.at(asset_path)->alias.c_str())){

        for(const auto& [k_asset_path, asset] : _program_state->asset_manager.textures){

            float max_width = 50.0f;
            float scale = 1.0f;

            if(asset->texture->w > max_width) scale = max_width/asset->texture->w; 

            bool pressed = ImGui::ImageButton(
                ("###ClassIconUniqueEntry"+asset->alias).c_str(),
                (ImTextureID)(intptr_t)asset->texture,
                ImVec2(asset->texture->w*scale, asset->texture->h*scale));

            ImGui::SameLine();

            bool is_selected = k_asset_path == asset_path;

            if(ImGui::Selectable(asset->alias.c_str(), is_selected)){
                asset_path = k_asset_path;
            }
            if(is_selected){
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    
    }
}

JsonDictionary PropertyAssetString::AsJson(){
    JsonDictionary j = JsonDictionary();
    j.Set("name", name);
    j.Set("value", asset_path);
    j.Set("type", "AssetString");
    return j;
}

//PropertyAnimation

PropertyAnimation::PropertyAnimation(std::string _name, AnimationEditorObject* _animation_editor_object) : Property(_name), animation_editor_object{_animation_editor_object}{
    
}

void PropertyAnimation::Update(ProgramState* _program_state){
    
}

JsonDictionary PropertyAnimation::AsJson(){
    JsonDictionary j = JsonDictionary();
    j.Set("name", name);
    //j.Set("value", )
    JsonArray j_animations = JsonArray();

    for(const auto& animation : animation_editor_object->animations){
        Console::PrintLine("Animations:",animation->key->asset_path);
    }

    Console::PrintLine("PropertyAnimation");

    j.Set("type", "Animation");
    return j;
}

//PropertyFloat

PropertyFloat::PropertyFloat(std::string _name, float _value) : Property(_name), value{_value}{}

void PropertyFloat::Update(ProgramState* _program_state){
    ImGui::InputFloat((name+"##"+std::to_string(id)).c_str(), &value);
}

JsonDictionary PropertyFloat::AsJson(){
    JsonDictionary j = JsonDictionary();
    j.Set("name", name);
    j.Set("value", value);
    j.Set("type", "float");
    return j;
}

//PropertyVector2f
PropertyVector2f::PropertyVector2f(std::string _name, float _x, float _y) : Property(_name), x{_x}, y{_y}{}

void PropertyVector2f::Update(ProgramState* _program_state){
    ImGui::InputFloat((name+".x"+"##"+std::to_string(id)).c_str(), &x);
    ImGui::InputFloat((name+".y"+"##"+std::to_string(id)).c_str(), &y);
}

JsonDictionary PropertyVector2f::AsJson(){
    JsonDictionary j = JsonDictionary();
    j.Set("name", name);
    j.Set("x", x);
    j.Set("y", y);
    j.Set("type", "Vector2f");
    return j;
}

}