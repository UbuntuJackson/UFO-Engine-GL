#pragma once
#include <camera.h>
#include <control_settings.h>
#include <level.h>
#include <engine.h>

class ControllableCamera : public Camera{
public:

    ufo::Controls* controls = nullptr;

    ControllableCamera(Vector2f _local_position) : Camera(_local_position){
        class_name = "ControllableCamera";
        editor_name = "@InstanceControlableCamera";
        is_savable = false;
    }

    void OnSpawn(){
        Camera::OnSpawn();
        clamp = false;

        level->active_camera_handles.push_back(this);

        controls = level->GetControls();
    }

    void OnUpdate(float _delta_time){
        Camera::OnUpdate(_delta_time);

        if(engine->mouse.is_middle_button_held){
            Vector2f delta_mouse = engine->mouse.former_position - engine->mouse.position;

            local_position += delta_mouse;
        }

        /*if(engine->keyboard.GetKey(controls->GetControl("right")).is_held) local_position.x += _delta_time * 100.0f;
        if(engine->keyboard.GetKey(controls->GetControl("left")).is_held) local_position.x -= _delta_time * 100.0f;
        if(engine->keyboard.GetKey(controls->GetControl("up")).is_held) local_position.y -= _delta_time * 100.0f;
        if(engine->keyboard.GetKey(controls->GetControl("down")).is_held) local_position.y += _delta_time * 100.0f;*/
    }

    void OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab){

    }

};
