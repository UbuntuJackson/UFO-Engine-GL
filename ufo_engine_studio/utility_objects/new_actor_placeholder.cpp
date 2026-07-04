#include "../../src/actor.h"
#include "../../src/input.h"
#include "../level_editor_tab.h"
#include "../editor.h"
#include "../../src/engine.h"
#include "../../ufo_maths/ufo_maths.h"
#include "new_actor_placeholder.h"

namespace UFOEngineStudio{

NewActorPlaceHolder::NewActorPlaceHolder(Vector2f _) : ufo::Actor(_){}

void NewActorPlaceHolder::OnSpawn(){
    editor_name = "Awaiting ...";
    class_name = "";
    is_savable = false;

}

void NewActorPlaceHolder::OnUpdate(float _dt){

}

ufo::Actor* NewActorPlaceHolder::OnGetFocusedActor(Vector2f _mouse_position_over_screenspace){
    return nullptr;
}

void NewActorPlaceHolder::OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    if(_level_editor_tab->actor_dedicated_to_viewport != this->editor_id){
        is_dead = true;
        level->actors_with_stable_id.erase(editor_id);
    }
}

}
