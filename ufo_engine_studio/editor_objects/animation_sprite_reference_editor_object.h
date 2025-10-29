#pragma once

#include <memory>
#include "../vector2f.h"
#include "editor_object.h"

class PropertyFloat;
class PropertyAssetString;
class PropertyVector2f;
class PropertyInt;

namespace UFOEngineStudio{

class AnimationSpriteReferenceEditorObject : public ActorEditorObject{
public:
    PropertyAssetString* key = nullptr;
    PropertyVector2f* offset = nullptr;
    PropertyVector2f* frame_size = nullptr;
    PropertyVector2f* scale = nullptr;
    PropertyFloat* rotation = nullptr;
    PropertyFloat* frame_index = nullptr;
    PropertyFloat* pace = nullptr;

    AnimationSpriteReferenceEditorObject();

    void OnSetupProperty(PropertyFloat *_p);

    void OnSetupProperty(PropertyAssetString* _p);

    void OnSetupProperty(PropertyString* _p);

    void OnSetupProperty(PropertyVector2f* _p);

    void OnSetupProperty(PropertyInt* _p);

    void Update(ProgramState* _program_state, ActorEditorObject* _parent_editor_object, Vector2f _parent_position);

};

}