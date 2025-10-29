#pragma once
#include <vector>
#include <memory>
#include "editor_object.h"
#include "../property.h"
#include "animation_sprite_reference_editor_object.h"

namespace UFOEngineStudio{

class ProgramState;

class AnimationEditorObject : public ActorEditorObject{
public:

    bool edit_animations = false;
    bool preview_on = false;

    AnimationSpriteReferenceEditorObject* currently_viewed_animation = nullptr;

    //The animation that the actor starts with
    std::string start_animation_asset_path = "";

    std::vector<std::unique_ptr<AnimationSpriteReferenceEditorObject>> animations;

    PropertyAnimation* property_animations = nullptr;

    void PropertyUpdate(ProgramState* _program);

    void Update(ProgramState* _program_state, ActorEditorObject* _parent_editor_object, Vector2f _parent_position);

    AnimationEditorObject();

};

}