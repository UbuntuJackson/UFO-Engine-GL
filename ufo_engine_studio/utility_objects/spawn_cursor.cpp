#include "../../src/actor.h"
#include "../../src/input.h"
#include "../level_editor_tab.h"
#include "../editor.h"
#include "../../src/engine.h"
#include "../../ufo_maths/ufo_maths.h"
#include "spawn_cursor.h"

namespace UFOEngineStudio{

SpawnCursor::SpawnCursor(Vector2f _) : Actor(_){}

void SpawnCursor::OnUpdate(float _dt){

}

Actor* SpawnCursor::OnGetFocusedActor(Vector2f _mouse_position_over_screenspace){
    return nullptr;
}

bool SpawnCursor::OnUpdateEditorViewportFocus(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    return false;
}

}
