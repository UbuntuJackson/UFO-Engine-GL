#include <memory>
#include <SDL3/SDL.h>
#include "animation_editor_object.h"
#include "../property.h"
#include "../program_state.h"
#include "../../imgui/imgui.h"
#include "animation_sprite_reference_editor_object.h"

namespace UFOEngineStudio{

AnimationEditorObject::AnimationEditorObject(){
    constructor_properties.clear();

    local_position_handle = AddConstructorProperty<PropertyVector2f>("local position", 0.0f, 0.0f);
    property_animations = AddProperty<PropertyAnimation>("animations", this);


}

void AnimationEditorObject::PropertyUpdate(ProgramState* _program){
    ActorEditorObject::PropertyUpdate(_program);

    if(ImGui::Button("Edit Animations")){
        edit_animations = true;
    }

    if(edit_animations){
        ImGui::Begin("Animation frames", &edit_animations);
        
        if(start_animation_asset_path != ""){

            if(ImGui::BeginCombo("Start Animation:", "...")){

                /*ImGui::SameLine();
                {
                    TextureWrapper* asset = _program->asset_manager.textures.at(start_animation_asset_path).get();

                    float max_width = 50.0f;
                    float scale = 1.0f;
        
                    if(asset->texture->w > max_width) scale = max_width/asset->texture->w; 

                    ImGui::ImageButton(
                        ("###ClassIconUniqueEntry"+asset->alias).c_str(),
                        (ImTextureID)(intptr_t)asset->texture,
                        ImVec2(asset->texture->w*scale, asset->texture->h*scale));
                }*/

                for(const auto& animation : animations){
        
                    TextureWrapper* asset = _program->asset_manager.textures.at(animation->key->asset_path).get();

                    float max_width = 50.0f;
                    float scale = 1.0f;
        
                    if(asset->texture->w > max_width) scale = max_width/asset->texture->w; 
        
                    bool pressed = ImGui::ImageButton(
                        ("###ClassIconUniqueEntry"+asset->alias).c_str(),
                        (ImTextureID)(intptr_t)asset->texture,
                        ImVec2(asset->texture->w*scale, asset->texture->h*scale));
        
                    ImGui::SameLine();
        
                    bool is_selected = animation->key->asset_path == start_animation_asset_path;
        
                    if(ImGui::Selectable(asset->alias.c_str(), is_selected)){
                        start_animation_asset_path = animation->key->asset_path;
                    }
                    if(is_selected){
                        ImGui::SetItemDefaultFocus();
                    }
                }
        
                ImGui::EndCombo();
            
            }
        }
        
        //Button to add animations
        if(ImGui::Button("Add Animation Sheet")){
            animations.push_back(std::make_unique<AnimationSpriteReferenceEditorObject>());
            if(start_animation_asset_path == "") start_animation_asset_path = animations.back()->key->asset_path;
        }

        //Preview the currently shown animation
        ImGui::Checkbox("Preview", &preview_on);

        int remove_at_index = -1;

        for(int i = 0; i < animations.size(); i++){
            
            ImGui::TextUnformatted(std::string("-- " + animations[i]->key->asset_path + " --").c_str());
            if(ImGui::IsItemClicked()){
                currently_viewed_animation = animations[i].get();
            }
            ImGui::SameLine();
            if(ImGui::Button("x")){
                remove_at_index = i;
            }
            animations[i]->PropertyUpdate(_program);
        }

        if(remove_at_index != -1){
            animations.erase(animations.begin() + remove_at_index);
            if(animations.size() > 0){
                currently_viewed_animation = animations[0].get();
                start_animation_asset_path = animations[0]->key->asset_path;
            }
            else{
                currently_viewed_animation = nullptr;
                start_animation_asset_path = "";
            }
        }

        ImGui::End();
    }
}

void AnimationEditorObject::Update(ProgramState* _program_state, ActorEditorObject* _parent_editor_object, Vector2f _parent_position){

    auto& io = ImGui::GetIO();

    if(currently_viewed_animation != nullptr) if(preview_on) currently_viewed_animation->frame_index->value += currently_viewed_animation->pace->value * io.DeltaTime;
    if(currently_viewed_animation != nullptr) currently_viewed_animation->Update(_program_state, _parent_editor_object, _parent_position);
    

}

}