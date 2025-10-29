#pragma once
#include "editor_object.h"

namespace UFOEngineStudio{

class ProgramState;

class TileMapEditorObject : public ActorEditorObject{
public:
    TileMapEditorObject(){

    }

    void Update(ProgramState* _program_state, ActorEditorObject* _parent_editor_object, Vector2f _parent_position);

};

}