#include "../../src/control_settings.h"
#include "../../src/graphics.h"
#include "../../src/level.h"
#include "../../src/engine.h"
#include "../../src/input.h"
#include "../../src/camera.h"
#include "../level_editor_tab.h"
#include "../editor.h"
#include "controllable_camera.h"

namespace UFOEngineStudio{

ControllableCamera::ControllableCamera(Vector2f _local_position) : Camera(_local_position){
    class_name = "ControllableCamera";
    editor_name = "ControllableCamera (Editor Tool)";
    is_savable = false;
}

void ControllableCamera::OnSpawn(){
    Camera::OnSpawn();
    clamp = false;

    level->active_camera_handles.push_back(this);

    controls = level->GetControls();
}

void ControllableCamera::OnUpdate(float _delta_time){
    Camera::OnUpdate(_delta_time);

    //Console::PrintLine("Controllable camera", local_position);

    if(engine->mouse.is_middle_button_held){
        Vector2f delta_mouse = engine->mouse.former_position - engine->mouse.position;

        local_position += delta_mouse / scale;
    }

    //scale += (float)engine->mouse.mouse_wheel*0.25f;
    //if(scale < 1.0f) scale = 1.0f;

    if(engine->keyboard.GetKey(SDLK_PLUS).is_pressed){
        scale += (0.25f);
    }
    if(engine->keyboard.GetKey(SDLK_MINUS).is_pressed){
        scale -= (0.25f);
        if(scale < 1.0f) scale = 1.0f;
    }

    /*if(engine->keyboard.GetKey(controls->GetControl("right")).is_held) local_position.x += _delta_time * 100.0f;
    if(engine->keyboard.GetKey(controls->GetControl("left")).is_held) local_position.x -= _delta_time * 100.0f;
    if(engine->keyboard.GetKey(controls->GetControl("up")).is_held) local_position.y -= _delta_time * 100.0f;
    if(engine->keyboard.GetKey(controls->GetControl("down")).is_held) local_position.y += _delta_time * 100.0f;*/
}

Actor* ControllableCamera::OnGetFocusedActor(Vector2f _mouse_position_over_screenspace){

    return nullptr;
}

void ControllableCamera::OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab){

}

bool ControllableCamera::UpdateEditorViewportFocus(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    return false;
}

void ControllableCamera::OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){

}

}
